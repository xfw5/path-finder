#include "PathFinding.h"
#include "GridNode.h"

PathFinder* PathFinder::getInstance()
{
	static PathFinder _sharePathFinder;

	return &_sharePathFinder;
}

void PathFinder::distoryInstance()
{
	
}

PathFinder::PathFinder()
{
	_nodes = nullptr;
	_width = 0;
	_height = 0;

	_openList.clear();
	_closeList.clear();
	_neighborsList.clear();
}

PathFinder::~PathFinder()
{
	if (_nodes) {
		for (auto i = 0; i < _height; i++) {
			delete _nodes[i];
		}

		delete _nodes;
	}
}

bool PathFinder::initMap(int width, int height)
{
	_nodes = new GridNode*[height];

	for (auto i = 0; i < height; i++) {
		_nodes[i] = new GridNode();
	}

	_width = width;
	_height = height;

	return true;
}

bool PathFinder::initMap(int width, int height, int *walkableMap[])
{
	_nodes = new GridNode *[height];

	for (auto i = 0; i < height; i++) {
		_nodes[i] = new GridNode [width];
	}

	for (auto i = 0; i < width; i++) {
		for (auto j = 0; j < height; j++) {
			_nodes[i][j]._walkable = walkableMap[i][j];
		}
	}

	return true;
}

bool PathFinder::astarSearch(GridNode *begin, GridNode *target)
{
	if (begin == nullptr || target == nullptr) return false;

	_openList.clear();
	_closeList.clear();

	if (begin == target) return true;

	_openList.push_back(begin);

	return doAstarSearch(begin, target);
}

bool PathFinder::doAstarSearch(GridNode *begin, GridNode *target)
{
	/*while (!_openList.empty()) {
		auto current = _openList.front();

		if (current == target) return true;

		auto neightbors = getNeighbors(current);
		for (auto next : neightbors) {
			if (isInCloseList(next) || !next->_isWalkable) continue;

			if (isInOpenList(next)) {

			}

			next->setFCost();
			next->setParent(current);
			_openList.push_back(next);

			if (next == target) return true;
		}
	}*/

	return false;
}

bool PathFinder::isInOpenList(GridNode *target)
{
	for (auto it : _openList) {
		if (target == it) return true;
	}

	return false;
}

bool PathFinder::isInCloseList(GridNode *target)
{
	for (auto it : _closeList) {
		if (target == it) return true;
	}

	return false;
}

//std::vector<GirdNode*>& PathFinder::getNeighbors(GirdNode *current)
//{
//	auto r = current->_x;
//	auto c = current->_y;
//	
//
//}