#include "HPAMap.h"

HPAMap::HPAMap()
:_shareGrid(nullptr)
, _finder(nullptr)
, _height(0)
, _width(0)
{
	memset(_gridClusters, 0, sizeof(_gridClusters));
}

HPAMap::~HPAMap()
{
	if (_shareGrid) {
		delete _shareGrid;
		_shareGrid = nullptr;
	}

	for (auto i = 0; i < MAP_SPLIT_ROW_NUM; i++) {
		for (auto j = 0; j < MAP_SPLIT_COL_NUM; j++) {
			if (_gridClusters[i][j]) {
				delete _gridClusters[i][j];
				_gridClusters[i][j] = nullptr;
			}
		}
	}
}
HPAMap* HPAMap::loading(int width, int height, int splitCount)
{
	auto map = new (std::nothrow) HPAMap();
	if (map && map->init(width, height, splitCount)) {
		return map;
	}

	delete map;
	map = nullptr;
	return nullptr;
}


bool HPAMap::init(int width, int height, int splitCount)
{
	_width = width;
	_height = height;
	_splitCount = splitCount;

	_shareGrid = Grid::create(width, height, 0, 0);
	
	buildClusters(MAP_SPLIT_ROW_NUM, MAP_SPLIT_COL_NUM);

	buildEntrances();

	return true;
}

void HPAMap::buildClusters(int rowNum, int colNum)
{
	auto width = _shareGrid->_totalCol / colNum;
	auto height = _shareGrid->_totalRow / rowNum;

	for (auto i = 0; i < rowNum; i++) {
		for (auto j = 0; j < colNum; j++) {
			_gridClusters[i][j] = new HPAGrid();
			_gridClusters[i][j]->_rowIndex = i;
			_gridClusters[i][j]->_colIndex = j;
			_gridClusters[i][j]->_grid.shareClone(*_shareGrid, j*width, i*height, width, height);
		}
	}
}

void HPAMap::buildEntrances()
{
	_shareGrid->initClearance();

	/*
	* List each clusters, find the max clearance
	* for each transition point.
	*/
	for (auto i = 0; i < MAP_SPLIT_ROW_NUM; i++) {
		for (auto j = 0; j < MAP_SPLIT_COL_NUM; j++) {
			findTransitionPoint(i,j);
		}
	}
}

void HPAMap::findTransitionPoint(int rowIndex, int colIndex)
{
	auto &hpaGrid = _gridClusters[rowIndex][colIndex];
	auto &subGrid = hpaGrid->_grid;

	// bottom
	if (_shareGrid->isWalkable(subGrid._xBase, subGrid._yBase-1)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalCol; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase+i, subGrid._yBase);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance  = markNode;
		hpaGrid->_adjacentClusters[Grid::AdjacentClustersType::ACT_BOTTOM] = \
			&_gridClusters[rowIndex-1][colIndex]->_grid;
	}

	//right
	if (_shareGrid->isWalkable(subGrid._xBase + subGrid._totalCol, subGrid._yBase)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase + subGrid._totalCol-1, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalRow; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase + subGrid._totalCol-1, subGrid._yBase+i);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance = markNode;
		hpaGrid->_adjacentClusters[Grid::AdjacentClustersType::ACT_RIGHT] = \
			&_gridClusters[rowIndex][colIndex+1]->_grid;
	}

	//top
	if (_shareGrid->isWalkable(subGrid._xBase, subGrid._yBase + subGrid._totalRow)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase+subGrid._totalRow-1);

		for (auto i = 1; i < subGrid._totalCol; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase+i, subGrid._yBase+subGrid._totalRow-1);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance = markNode;
		hpaGrid->_adjacentClusters[Grid::AdjacentClustersType::ACT_TOP] = \
			&_gridClusters[rowIndex + 1][colIndex]->_grid;
	}

	//left
	if (_shareGrid->isWalkable(subGrid._xBase -1, subGrid._yBase)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalRow; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase+i);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance = markNode;
		hpaGrid->_adjacentClusters[Grid::AdjacentClustersType::ACT_BOTTOM] = \
			&_gridClusters[rowIndex][colIndex-1]->_grid;
	}
}
