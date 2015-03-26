/*
* Node for path finder.
*
* XFW, 2015.
*/

#ifndef __ASTAR_CELL_H__
#define __ASTAR_CELL_H__

class GridNode{
public:
	GridNode()
	{
		_close = false;
		_open = false;

		_x = _y = _f = _g = _h = 0;

		_clearance = 1;

		_walkable = false;
		_parent = nullptr;
	}

	GridNode(int x, int y, bool w)
	{
		GridNode();

		_x = x;
		_y = y;
		_walkable = w;
	}

	void reset()
	{
		_walkable = true;

		clear();
	}

	void clear()
	{
		_open = false;
		_close = false;

		_f = _g = _h = 0;

		_clearance = 1;

		_parent = nullptr;
	}

	friend bool operator < (const GridNode &n1, const GridNode &n2)
	{
		return n1._f < n2._f;
	}

public:
	int _x;
	int _y;

	int _clearance; // for HPA path finder

	unsigned short _f; // f = g + h
	unsigned short _g;
	unsigned short _h;

	bool _walkable;
	bool _close;
	bool _open;

	GridNode *_parent;
};

#endif