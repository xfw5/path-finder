/*
* A star path finding implement.
*
* XFW, 2015.
*/

#ifndef __A_STAR_FINDER_H__
#define __A_STAR_FINDER_H__

#include "Grid.h"
#include <vector>
#include <stack>

class GridNode;

#define ASTAR_DEBUG_ON 1

class AstarFinder
{
public:
	class HeapCompare
	{
	public:
		bool operator() (GridNode *n1, GridNode *n2) const
		{
			return n1->_f > n2->_f;
		}
	};

	AstarFinder()
	{
		reset();
	}

	void reset()
	{
		_targetNode = nullptr;

		_openList.clear();
		_closeList.clear();

		_clearance = 0;

#if ASTAR_DEBUG_ON
		_isStep = false;
#endif
	}

	inline void backtrace(std::stack<GridNode*> *pathsStack) { backtrace(pathsStack, _targetNode); }
	inline void backtrace(std::vector<GridNode*> *pathsList) { backtrace(pathsList, _targetNode); }

	void backtrace(std::stack<GridNode*> *pathsStack, GridNode *node);
	void backtrace(std::vector<GridNode*> *pathsList, GridNode *node);

	bool findPath(int startX, int startY, int endX, int endY, int clearance, Grid *grid);

#if ASTAR_DEBUG_ON
	bool findPathStepInit(int startX, int startY, int endX, int endY, int clearance, Grid *grid);
	void findPathStep(Grid *grid);
#endif

	std::vector<GridNode*> _openList;
	std::vector<GridNode*> _closeList;

	int _clearance;

	GridNode *_targetNode;
	GridNode *_sourceNode;

	bool _isStep;
};

#endif