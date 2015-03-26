#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Grid.h"
#include "HPAFinder.h"

USING_NS_CC;

class PathFinderTest : public cocos2d::LayerColor
{
public:
    virtual bool init();  

    static cocos2d::Scene* scene();

	void update(float detla);

	void onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);

	PathFinderTest();
	~PathFinderTest();
private:

    CREATE_FUNC(PathFinderTest);

	DrawNode *_drawNode;
	DrawNode *_drawNodeGrid;

	float _cellSize;
	int _col;
	int _row;

	HPAFinder *_finder;

	Label *_debugInfoLabel;
	Node *_debugNode;
	Label *_reset;

	char _cellMsg[34];

	Label *_pathFinding;
};

#endif // __HELLOWORLD_SCENE_H__
