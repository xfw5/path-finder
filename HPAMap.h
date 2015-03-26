#ifndef __HPA_MAP_H__
#define __HPA_MAP_H__

#include "PathFinderConfig.h"
#include "Grid.h"
#include "HPAFinder.h"

class HPAMap
{
public:

	static HPAMap* loading(int width, int height, int splitCount);

	HPAMap();
	~HPAMap();

protected:
	bool init(int width, int height, int splitCount);

	void buildClusters(int rowNum, int colNum);

	void buildEntrances();

	void findTransitionPoint(int rowNum, int colNum);

public:
	int _width;
	int _height;

	int _splitCount;

	HPAFinder *_finder;

	Grid *_shareGrid;
	HPAGrid *_gridClusters[MAP_SPLIT_ROW_NUM][MAP_SPLIT_COL_NUM];
};

#endif