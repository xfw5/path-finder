#include "AstarFinder.h"
#include "Grid.h"
#include <algorithm>

bool AstarFinder::findPath(int startX, int startY, int endX, int endY, int clearance, Grid *grid)
{
	if (!grid->isInbound(startX, startY) || !grid->isInbound(endX, endY)) return false;

	_sourceNode = grid->getNodeSafe(startX, startY);
	_targetNode = grid->getNodeSafe(endX, endY);

	/*
	* Make sure the target node is under travel-able.
	*/
	if (_targetNode->_clearance < clearance) return false;

	_sourceNode->_open = true;
	_openList.push_back(_sourceNode);
	_closeList.clear();

	push_heap(_openList.begin(), _openList.end(), HeapCompare());

	while (!_openList.empty()) {
		auto current = _openList.front();
		pop_heap(_openList.begin(), _openList.end(), HeapCompare());
		_openList.pop_back();

		current->_close = true;
		_closeList.push_back(current);

		if (current == _targetNode) {
			_openList.clear();
			return true;
		}

		auto neighbors = grid->getNeighbors(*current, clearance);
		for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
			auto neighbor = (*it);
			if (neighbor->_close) continue;

			auto &x = neighbor->_x;
			auto &y = neighbor->_y;

			auto gScore = current->_g + ((x == current->_x || y == current->_y) ? 1 : 1.4);

			if (!neighbor->_open || gScore < neighbor->_g) {
				neighbor->_g = gScore;
				neighbor->_h = abs(neighbor->_x - _targetNode->_x) + abs(neighbor->_y - _targetNode->_y);
				neighbor->_f = neighbor->_g + neighbor->_h;
				neighbor->_parent = current;

				if (!neighbor->_open) {
					neighbor->_open = true;
					_openList.push_back(neighbor);
					push_heap(_openList.begin(), _openList.end(), HeapCompare());
				}
				else {
					auto itr = _openList.begin();
					for (; itr != _openList.end(); itr++) {
						if (*itr == *it) {
							_openList.erase(itr);
							break;
						}
					}
				}
			}
		}
	}

	return false;
}

void AstarFinder::backtrace(std::stack<GridNode*> *pathsStack, GridNode *node)
{
	if (!node) return;

	while (!pathsStack->empty())
		pathsStack->pop();

	pathsStack->push(node);

	while (node->_parent) {
		node = node->_parent;
		pathsStack->push(node);
	}
}

void AstarFinder::backtrace(std::vector<GridNode*> *pathsList, GridNode *node)
{
	pathsList->clear();

	pathsList->push_back(node);

	while (node->_parent) {
		node = node->_parent;
		pathsList->push_back(node);
	}

	std::reverse(pathsList->begin(), pathsList->end());
}

#if ASTAR_DEBUG_ON
bool AstarFinder::findPathStepInit(int startX, int startY, int endX, int endY, int clearance, Grid *grid)
{
	_sourceNode = grid->getNodeSafe(startX, startY);
	_targetNode = grid->getNodeSafe(endX, endY);

	if (_targetNode->_clearance < clearance) return false;

	_sourceNode->_open = true;
	_openList.push_back(_sourceNode);

	push_heap(_openList.begin(), _openList.end(), HeapCompare());

	_clearance = clearance;
	_isStep = true;

	return true;
}

void AstarFinder::findPathStep(Grid *grid)
{
	if (!_openList.empty()) {
		auto current = _openList.front();

		pop_heap(_openList.begin(), _openList.end(), HeapCompare());
		_openList.pop_back();

		current->_close = true;
		_closeList.push_back(current);

		if (current == _targetNode) {
			return;
		}

		auto neighbors = grid->getNeighbors(*current, _clearance);
		for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
			auto neighbor = (*it);
			if (neighbor->_close) continue;

			auto &x = neighbor->_x;
			auto &y = neighbor->_y;

			auto gScore = current->_g + ((x == current->_x || y == current->_y) ? 1 : 1.4);

			if (!neighbor->_open || gScore < neighbor->_g) {
				neighbor->_g = gScore;
				neighbor->_h = abs(neighbor->_x - _targetNode->_x) + abs(neighbor->_y - _targetNode->_y);
				neighbor->_f = neighbor->_g + neighbor->_h;
				neighbor->_parent = current;

				if (!neighbor->_open) {
					neighbor->_open = true;
					_openList.push_back(neighbor);
					push_heap(_openList.begin(), _openList.end(), HeapCompare());
				}
				else {
					_openList.erase(it);
				}
			}
		}
	}
}
#endif