/*
* Clearance-based path finding and hierarchical
* annotated A star search implement.
*
* Implement:
*  - With heap's algorithm support for fast access when calculating f-cost.
*
* XFW, 2015.
*/

#ifndef __HPA_FINDER_H__
#define __HPA_FINDER_H__

#include <vector>
#include "Grid.h"
#include "AstarFinder.h"
#include "PathFinderConfig.h"

#define HPA_DEBUG_ON 1

class HPAFinder
{
public:
	static HPAFinder* create(int width, int height);

	bool findPath(int startX, int startY, int endX, int endY, int clearance);

	void backtrace(std::stack<GridNode*> **pathsStack, int row, int col);

	inline Grid* getGrid(int x, int y) { return &_clusters[x][y]->_grid; }

	inline ClusterGrid* getCluster(int x, int y) { return _clusters[x][y]; }

	HPAFinder();
	~HPAFinder();

protected:
	bool init(int width, int height);

	void buildClusters(int rowNum, int colNum);

	void buildEntrances();

	void buildEdge();

	void findTransitionPoint(int rowNum, int colNum);

	ClusterGrid* getGridByPos(int xPos, int yPos);

private:

	Grid *_shareGrid;

	AstarFinder _astarFinders[MAP_SPLIT_ROW_NUM][MAP_SPLIT_COL_NUM];

	ClusterGrid *_clusters[MAP_SPLIT_ROW_NUM][MAP_SPLIT_COL_NUM];

	std::vector<GridNode*> _edge;// TODO

	int _clusterWidth;
	int _clusterHeight;
};
#endif