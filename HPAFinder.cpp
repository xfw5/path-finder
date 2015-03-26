#include "HPAFinder.h"
#include "Grid.h"

HPAFinder::HPAFinder()
:_shareGrid(nullptr)
{
	memset(_clusters, 0, sizeof(_clusters));
}

HPAFinder::~HPAFinder()
{
	if (_shareGrid) {
		delete _shareGrid;
		_shareGrid = nullptr;
	}

	for (auto i = 0; i < MAP_SPLIT_ROW_NUM; i++) {
		for (auto j = 0; j < MAP_SPLIT_COL_NUM; j++) {
			if (_clusters[i][j]) {
				delete _clusters[i][j];
				_clusters[i][j] = nullptr;
			}
		}
	}
}

HPAFinder* HPAFinder::create(int width, int height)
{
	auto hpa = new HPAFinder();
	if (hpa && hpa->init(width, height)) {
		return hpa;
	} else {
		delete hpa;
		hpa = nullptr;
		return nullptr;
	}
}

bool HPAFinder::init(int width, int height)
{
	_shareGrid = Grid::create(width, height, 0, 0);

	buildClusters(MAP_SPLIT_ROW_NUM, MAP_SPLIT_COL_NUM);

	buildEntrances();

	return true;
}

void HPAFinder::buildClusters(int rowNum, int colNum)
{
	_clusterWidth = _shareGrid->_totalCol / colNum;
	_clusterHeight = _shareGrid->_totalRow / rowNum;

	for (auto i = 0; i < rowNum; i++) {
		for (auto j = 0; j < colNum; j++) {
			_clusters[i][j] = new ClusterGrid();
			_clusters[i][j]->_rowIndex = i;
			_clusters[i][j]->_colIndex = j;
			_clusters[i][j]->_grid.shareClone(*_shareGrid, \
				j*_clusterWidth, i*_clusterHeight, _clusterWidth, _clusterHeight);
		}
	}
}

void HPAFinder::buildEntrances()
{
	_shareGrid->initClearance();

	/*
	* List each clusters, find the max clearance
	* for each transition point.
	*/
	for (auto i = 0; i < MAP_SPLIT_ROW_NUM; i++) {
		for (auto j = 0; j < MAP_SPLIT_COL_NUM; j++) {
			findTransitionPoint(i, j);
		}
	}
}

void HPAFinder::findTransitionPoint(int rowIndex, int colIndex)
{
	auto &hpaGrid = _clusters[rowIndex][colIndex];
	auto &subGrid = hpaGrid->_grid;

	// bottom
	if (_shareGrid->isWalkable(subGrid._xBase, subGrid._yBase - 1)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalCol; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase + i, subGrid._yBase);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance[ClusterGrid::AdjacentClustersType::ACT_BOTTOM] = markNode;
		hpaGrid->_adjacentClusters[ClusterGrid::AdjacentClustersType::ACT_BOTTOM] = \
			&_clusters[rowIndex - 1][colIndex]->_grid;
	}

	//right
	if (_shareGrid->isWalkable(subGrid._xBase + subGrid._totalCol, subGrid._yBase)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase + subGrid._totalCol - 1, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalRow; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase + subGrid._totalCol - 1, subGrid._yBase + i);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance[ClusterGrid::AdjacentClustersType::ACT_RIGHT] = markNode;
		hpaGrid->_adjacentClusters[ClusterGrid::AdjacentClustersType::ACT_RIGHT] = \
			&_clusters[rowIndex][colIndex + 1]->_grid;
	}

	//top
	if (_shareGrid->isWalkable(subGrid._xBase, subGrid._yBase + subGrid._totalRow)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase + subGrid._totalRow - 1);

		for (auto i = 1; i < subGrid._totalCol; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase + i, subGrid._yBase + subGrid._totalRow - 1);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance[ClusterGrid::AdjacentClustersType::ACT_TOP] = markNode;
		hpaGrid->_adjacentClusters[ClusterGrid::AdjacentClustersType::ACT_TOP] = \
			&_clusters[rowIndex + 1][colIndex]->_grid;
	}

	//left
	if (_shareGrid->isWalkable(subGrid._xBase - 1, subGrid._yBase)) {
		auto markNode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase);

		for (auto i = 1; i < subGrid._totalRow; i++) {
			auto inode = _shareGrid->getNode(subGrid._xBase, subGrid._yBase + i);
			if (inode->_clearance > markNode->_clearance)
				markNode = inode;
		}

		hpaGrid->_transitionEntrance[ClusterGrid::AdjacentClustersType::ACT_LEFT] = markNode;
		hpaGrid->_adjacentClusters[ClusterGrid::AdjacentClustersType::ACT_LEFT] = \
			&_clusters[rowIndex][colIndex - 1]->_grid;
	}
}

void HPAFinder::buildEdge()
{
	for (auto i = 0; i < ClusterGrid::AdjacentClustersType::ACT_MAX; i++) {
		
	}
}

bool HPAFinder::findPath(int startX, int startY, int endX, int endY, int clearance)
{
	auto starGrid = getGridByPos(startX, startY);
	auto endGrid = getGridByPos(endX, endY);

	bool found = false;

	/*
	* find path in start point location cluster: start point to each entrance.
	*/
	for (auto i = 0; i < ClusterGrid::AdjacentClustersType::ACT_MAX; i++) {
		auto entrance = starGrid->_transitionEntrance[i];
		if (entrance) {
			found = _astarFinders[starGrid->_rowIndex][starGrid->_colIndex].findPath(\
				startX, startY, entrance->_x, entrance->_y, clearance, &starGrid->_grid);
		}
	}

	/*
	* start path is block, return immediately.
	*/
	if (!found) return false;

	/*
	* find path in end point location cluster: end point to each entrance.
	*/
	for (auto i = 0; i < ClusterGrid::AdjacentClustersType::ACT_MAX; i++) {
		auto entrance = endGrid->_transitionEntrance[i];
		if (entrance) {
			found = _astarFinders[endGrid->_rowIndex][endGrid->_colIndex].findPath(\
				endX, endY, entrance->_x, entrance->_y, clearance, &endGrid->_grid);
		}
	}

	return found;
}

void HPAFinder::backtrace(std::stack<GridNode*> **pathsStack, int row, int col)
{
	for (auto i = 0; i < MAP_SPLIT_ROW_NUM; i++) {
		for (auto j = 0; j < MAP_SPLIT_COL_NUM; j++)
			_astarFinders[i][j].backtrace(pathsStack[i*MAP_SPLIT_ROW_NUM+j]);
	}
}

ClusterGrid* HPAFinder::getGridByPos(int xPos, int yPos)
{
	auto row = xPos / _clusterHeight;
	auto col = yPos / _clusterWidth;

	return _clusters[row][col];
}

