#include <vector>
#ifndef __PATH_FINDING_H__
#define __PATH_FINDING_H__

class GridNode;

class PathFinder
{
public:
	static PathFinder* getInstance();
	static void distoryInstance();

	bool initMap(int width, int height);
	bool initMap(int width, int height, int *walkableMatrix[]);

	bool astarSearch(GridNode *begin, GridNode *target);

	PathFinder();
	~PathFinder();
protected:

	bool doAstarSearch(GridNode *begin, GridNode *target);

	bool isInOpenList(GridNode *target);
	bool isInCloseList(GridNode *target);

	std::vector<GridNode*>& getNeighbors(GridNode *current);
private:
	std::vector<GridNode*> _openList;
	std::vector<GridNode*> _closeList;

	std::vector<GridNode*> _neighborsList;

	GridNode **_nodes;

	int _width;
	int _height;
};

#endif