#include "Grid.h"

Grid::Grid()
{
	_totalCol = 0;
	_totalRow = 0;

	_xBase = _yBase = 0;

	_shareNodes = true;
	_nodes = nullptr;
	_neighbors.clear();
}

Grid::~Grid()
{
	if (!_shareNodes && _nodes) {
		for (auto i = 0; i < _totalCol; i++)
			delete _nodes[i];

		delete[] _nodes;
		_nodes = nullptr;
	}
}

Grid* Grid::create(int width, int height, bool *walkableMap[])
{
	auto g = new (std::nothrow) Grid();
	if (g && g->init(width, height, walkableMap)) {
		return g;
	}

	delete g;
	g = nullptr;
	return nullptr;
}

Grid* Grid::create(int width, int height, int xBase, int yBase)
{
	auto g = new (std::nothrow) Grid();
	if (g && g->init(width, height, xBase, yBase)) {
		return g;
	}

	delete g;
	g = nullptr;
	return nullptr;
}

bool Grid::init(int width, int height, bool *walkableMap[])
{
	if (!createNodes(width, height)) return false;

	for (auto i = 0; i < height; i++) {
		for (auto j = 0; j < width; j++) {
			(_nodes[i][j])._x = i;
			(_nodes[i][j])._y = j;
			_nodes[i][j]._walkable = walkableMap[i][j];
		}
	}

	_totalCol = width;
	_totalRow = height;

	_xBase = _yBase = 0;

	_neighbors.clear();

	return true;
}

bool Grid::init(int width, int height, int xBase, int yBase)
{
	if (!createNodes(width, height)) return false;

	for (auto i = 0; i < width; i++) {
		for (auto j = 0; j < height; j++) {
			(_nodes[i][j])._x = xBase + i;
			(_nodes[i][j])._y = yBase + j;
			(_nodes[i][j])._walkable = true;
		}
	}

	_totalCol = width;
	_totalRow = height;

	_xBase = xBase;
	_yBase = yBase;

	return true;
}

bool Grid::createNodes(int width, int height)
{
	_nodes = new GridNode *[width];
	if (!_nodes) return false;

	for (auto i = 0; i < width; i++) {
		_nodes[i] = new GridNode[height];
		if (!_nodes[i]) {
			for (auto j = 0; j < i; j++)
				delete _nodes[j];

			delete[] _nodes;
			_nodes = nullptr;
			return false;
		}
	}

	_shareNodes = false;
	return true;
}

void Grid::reset()
{
	for (auto i = 0; i < _totalRow; ++i) {
		for (auto j = 0; j < _totalCol; ++j) {
			(_nodes[i][j]).reset();
		}
	}

	initClearance();
}

void Grid::clear()
{
	for (auto i = 0; i < _totalRow; ++i) {
		for (auto j = 0; j < _totalCol; ++j) {
			(_nodes[i][j]).clear();
		}
	}

	initClearance();
}

std::vector<GridNode*>& Grid::getNeighbors(const GridNode &node, int clearance)
{
	auto &x = node._x;
	auto &y = node._y;

	_neighbors.clear();

	// top
	if (isWalkable(x, y + 1, clearance)) {
		_neighbors.push_back(&_nodes[x][y + 1]);
	}

	// right
	if (isWalkable(x + 1, y, clearance)) {
		_neighbors.push_back(&_nodes[x + 1][y]);
	}

	// bottom
	if (isWalkable(x, y - 1, clearance)) {
		_neighbors.push_back(&_nodes[x][y - 1]);
	}

	// left
	if (isWalkable(x - 1, y, clearance)) {
		_neighbors.push_back(&_nodes[x - 1][y]);
	}

	return _neighbors;
}

void Grid::initClearance()
{
	for (auto i = 0; i < _totalCol; i++) {
		for (auto j = 0; j < _totalRow; j++) {
			(_nodes[i][j])._clearance = calcClearance(i, j);
		}
	}
}

int Grid::calcClearance(int col, int row)
{
	/*
	* Max clearance is the min-est of total len.
	*/
	auto maxClearance = _totalCol < _totalRow ? _totalCol : _totalRow;

	if (!isWalkable(col, row)) return 0;

	int i = 0;
	for (i = 1; i < maxClearance; i++) {
		// in row
		for (auto j = 0; j <= i; j++) {
			if (isWalkable(col + j, row - i)) {
				continue;
			}

			return i;
		}

		// in col
		for (auto j = 0; j <= i; j++) {
			if (isWalkable(col + i, row - j)) {
				continue;
			}

			return i;
		}
	}

	return i;
}

bool Grid::shareClone(const Grid &other, int xLocation, int yLocation, int width, int height)
{
	if (xLocation + width > other._totalCol || \
		yLocation + height > other._totalRow) return false;

	_totalCol = width;
	_totalRow = height;

	_xBase = xLocation;
	_yBase = yLocation;

	_nodes = other._nodes;

	_shareNodes = true;

	return true;
}
