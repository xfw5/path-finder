/*
* Path finder map's grid.
*
* Grid: origin point at the bottom left corner.
*  4 
*  3
*  2
*  1
*  0 1 2 3 4 ->
*
* XFW, 2015.
*/

#ifndef __GRID_H__
#define __GRID_H__

#include "GridNode.h"
#include <vector>

class HPAFinder;
class ClusterGrid;

class Grid
{
	friend class HPAFinder;
public:

	/*
	* width: col
	* height: row;
	*/
	static Grid* create(int width, int height, int xBase, int yBase);

	static Grid* create(int width, int height, bool *walkableMap[]);

	bool shareClone(const Grid &other, int xLocation, int yLocation, int width, int height);

	Grid();
	~Grid();

	/*
	* reset all grid node status, and init clearance.
	* Only for map's grid needed redraw.
	*/
	void reset();

	/*
	* Clear the last search status, for the next path finding.
	*/
	void clear();

	std::vector<GridNode*>& getNeighbors(const GridNode &node, int clearance);

	void initClearance();

	/*
	* Safely get node.
	*/
	inline GridNode* getNodeSafe(int x, int y)
	{
		if (!isInbound(x, y)) return nullptr;

		return getNode(x, y);
	}

	/*
	* Get node from base location for HPA search.
	*/
	inline GridNode* getNodeByBase(int x, int y)
	{
		x -= _xBase;
		y -= _yBase;

		return getNodeSafe(x, y);
	}

	inline bool isWalkable(int x, int y, int clearance) 
	{
		return isInbound(x, y) && \
			getNode(x,y)->_walkable && \
			getNode(x,y)->_clearance >= clearance; 
	}

	inline bool isWalkable(int x, int y) { return isInbound(x, y) && getNode(x, y)->_walkable; }

	inline void setWalkable(int x, int y, bool walkable) { getNode(x, y)->_walkable = walkable; }

	inline bool isInbound(int x, int y) { return (x >= 0 && x < _totalCol) && (y >= 0 && y < _totalRow); }

	inline const int& getTotalCol() const { return _totalCol; }
	inline const int& getTotalRow() const { return _totalRow; }

protected:
	bool init(int width, int height, bool *walkableMap[]);
	bool init(int width, int height, int xBase, int yBase);

	bool createNodes(int width, int height);

	inline GridNode* getNode(int x, int y) { return &_nodes[x][y]; }

	/*
	* Clearance base HPA calculating from top-left grid node.
	* How it work:
	*  - If all node in bounding box is walkable, then clearance increase by 1.
	*
	* The bounding box calculate from top-left to the bottom-right corner. Just like "¨K"
	* Cause the bounding box is calculating from top-left to bottom-right, the AI
	* bounding box must align with the left to right.
	*
	* Paper:
	*  - <<Principles of Robot Motion Theory, Algorithms, and Implementations>>
	*  - <<Hierarchical Path Planning for Multi-Size Agents in Heterogeneous Environments>>
	*/
	int calcClearance(int col, int row);

private:
	int _totalCol;
	int _totalRow;

	int _xBase;
	int _yBase;

	bool _shareNodes;
	GridNode **_nodes;

	std::vector<GridNode*> _neighbors;
};

class ClusterGrid
{
public:

	enum AdjacentClustersType{
		ACT_BOTTOM = 0,
		ACT_RIGHT,
		ACT_TOP,
		ACT_LEFT,

		ACT_MAX
	};

	ClusterGrid()
	{
		memset(_adjacentClusters, 0, sizeof(_adjacentClusters));
		memset(_transitionEntrance, 0, sizeof(_transitionEntrance));

		_rowIndex = 0;
		_colIndex = 0;
	}

	Grid _grid;

	GridNode *_transitionEntrance[ACT_MAX];

	Grid *_adjacentClusters[ACT_MAX];

	unsigned short _rowIndex;
	unsigned short _colIndex;
};

#endif