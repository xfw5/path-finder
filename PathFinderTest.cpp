#include "PathFinderTest.h"
#include "AppMacros.h"
#include "Grid.h"
#include "..\..\cpp-tests\Classes\VisibleRect.h"
#include "FrameBuffer.h"
#include "2d\CCRenderTexture.h"
#include "RenderTextureEx.h"
#include "..\..\..\Box2D\Common\b2Settings.h"

USING_NS_CC;

#define PRINT_CELL_INFO(cell) \
do {\
	snprintf(_cellMsg, sizeof(_cellMsg), "r:%d c:%d\nw:%s", \
	cell._x, cell._y, cell._walkable == true ? "true" : "false"); \
	_cellInfoLabel->setString(_cellMsg); \
} while (0);

Scene* PathFinderTest::scene()
{
    auto scene = Scene::create();

    PathFinderTest *layer = PathFinderTest::create();

    scene->addChild(layer);

    return scene;
}

bool PathFinderTest::init()
{
    if ( !LayerColor::initWithColor(Color4B::WHITE) )
    {
        return false;
    }

	auto visibleSize = Director::getInstance()->getVisibleSize();

    auto origin = Director::getInstance()->getVisibleOrigin();

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesEnded = CC_CALLBACK_2(PathFinderTest::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	_debugInfoLabel = Label::createWithSystemFont("Info", "Arial", 24);
	if (!_debugInfoLabel) return false;
	this->addChild(_debugInfoLabel);
	_debugInfoLabel->setColor(Color3B::MAGENTA);
	_debugInfoLabel->setZOrder(999);
	_debugInfoLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
	_debugInfoLabel->setPosition(visibleSize.width*0.5f, visibleSize.height);

	_reset = Label::createWithSystemFont("reset", "Arial", 24);
	if (!_reset) return false;
	this->addChild(_reset);
	_reset->setColor(Color3B::MAGENTA);
	_reset->setZOrder(999);
	_reset->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
	_reset->setPosition(visibleSize.width*0.5f, 0);

	_pathFinding = Label::createWithSystemFont("A star", "Arial", 24);
	if (!_pathFinding) return false;
	this->addChild(_pathFinding);
	_pathFinding->setColor(Color3B::MAGENTA);
	_pathFinding->setZOrder(999);
	_pathFinding->setPosition(visibleSize);
	_pathFinding->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);

	_cellMsg[0] = 0;

	auto w = visibleSize.width;
	auto h = visibleSize.height;

	_row = 10;
	_cellSize = h / _row;
	_col = (int)w / (int)_cellSize;

	_drawNodeGrid = DrawNode::create();
	this->addChild(_drawNodeGrid);

	_drawNode = DrawNode::create();
	this->addChild(_drawNode);

	for (auto i = 0; i < h; i += _cellSize) {
		_drawNodeGrid->drawLine(Vec2(0, i), Vec2(w, i), Color4F::BLACK);
	}

	for (auto i = 0; i < w; i += _cellSize) {
		_drawNodeGrid->drawLine(Vec2(i, 0), Vec2(i, h), Color4F::BLACK);
	}

	//_finder = HPAFinder::create(_row, _col);
	union testData{
		uint32 _key;
		
		struct {
			uint16 _id;
			uint16 _level;
		}IdLevel;
	};

	union testData data = {0};
	data.IdLevel._id = 100;
	data.IdLevel._level = 1;

	auto key = data._key;

	auto t1 = Vec3::angle(Vec3(1, 0, 0), Vec3(-1, 0, -1));
	auto t2 = Vec3::angle(Vec3(1, 0, 0), Vec3(-1, 0, 1));

	auto n = Vec3::ZERO;
	auto v1 = Vec3(1, 0, 0);
	auto v2 = Vec3(-1, 0, -1);
	Vec3::cross(v1, v2, &n);
	auto dot = Vec3::dot(v1, v2);

	auto v3 = Vec3(-1, 0, 1);
	auto n2 = Vec3::ZERO;
	Vec3::cross(v1, v3, &n);
	auto dot2 = Vec3::dot(v1, v3);

	this->scheduleUpdate();
    return true;
}

void PathFinderTest::update(float detla)
{
	auto size = Director::getInstance()->getVisibleSize();
	
}

void PathFinderTest::onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
	auto pos = touches[0]->getLocation();

#if 0
	//if (_pathFinding->getBoundingBox().containsPoint(pos)) {
	//	auto getit = _map->_finder->findPath(0, 0, 4 - 1, 5 -1, 1);
	//	if (!getit) return;

	//	std::stack<GridNode*> backtrace[4];
	//	_finder->printPath(&backtrace[0], 0, 0);
	//	//_finder->printPath(&backtrace[1], 0, 1);
	//	//_finder->printPath(&backtrace[2], 1, 0);
	//	//_finder->printPath(&backtrace[3], 1, 1);
	//	
	//	int i = 0;
	//	//for (auto i = 0; i < 4; i++) {
	//		while (!backtrace[i].empty()) {
	//			auto n = backtrace[i].top();
	//			backtrace[i].pop();
	//			_drawNode->drawSolidRect(Vec2(n->_x, n->_y)*_cellSize, Vec2(n->_x + 1, n->_y + 1)*_cellSize, Color4F::RED);
	//		}
	////	}

	//	//for (auto it : *_finder->getOpenList()) {
	//	//	_drawNode->drawSolidRect(Vec2(it->_y, it->_x)*_cellSize, Vec2(it->_y + 1, it->_x + 1)*_cellSize, Color4F::GREEN);
	//	//	//_drawNode->drawPoint(Vec2(it->_y, it->_x), _cellSize, Color4F::GREEN);
	//	//}

	//	//_finder->backtrace();
	//	//for (auto it : *_finder->getBacktraceList()) {
	//	//	_drawNode->drawSolidRect(Vec2(it->_y, it->_x)*_cellSize, Vec2(it->_y + 1, it->_x + 1)*_cellSize, Color4F::RED);
	//	//}

	//	return;
	//}

	if (_reset->getBoundingBox().containsPoint(pos)) {
	//	_grid.reset();
		_drawNode->clear();
		//_finder->reset();
		return;
	}

	if (_debugInfoLabel->getBoundingBox().containsPoint(pos)) {
		if (_debugNode) {
			_debugNode->removeFromParent();
			_debugNode = nullptr;
		}

		_debugNode = Node::create();
		this->addChild(_debugNode);
		_debugNode->setZOrder(2014);

		Color3B color[2][2] = { Color3B(250, 20, 45), Color3B(25, 240, 25), Color3B(125, 100, 245), Color3B(55, 40, 65) };
		for (auto row = 0; row < 2; row++)
		for (auto col = 0; col < 2; col++) {
			auto grid = _finder->getGrid(row, col);
			for (auto i = 0; i < grid->getTotalCol(); i++) {
				for (auto j = 0; j < grid->getTotalRow(); j++) {
					auto nd = grid->getNodeSafe(i, j);
					static char msg[4] = "\0";
					//snprintf(msg, sizeof(msg), "%d  %d", nd->_clearance, nd->_walkable?1:0);
					snprintf(msg, sizeof(msg), "%d", nd->_clearance);
					auto ll = Label::createWithSystemFont(msg, "Arial", 18);
					ll->setPosition(nd->_x*_cellSize, nd->_y*_cellSize);
					ll->setColor(color[row][col]);
					ll->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
					_debugNode->addChild(ll);
				}
			}
		}
		
		for (auto i = 0; i < 2; i++) {
			for (auto j = 0; j < 2; j++) {
				auto n = _finder->getCluster(i, j)->_transitionEntrance;
				_drawNode->drawSolidRect(Vec2(n->_x, n->_y)*_cellSize, Vec2(n->_x + 1, n->_y + 1)*_cellSize, Color4F::RED);
			}
		}

		return;
	}

	//pos = pos.operator/(_cellSize);
	//auto c = (int)pos.x;
	//auto r = (int)pos.y;

	//auto grid = _finder->getGrid(r / _finder->getGrid(0,0)->getTotalRow(), c / _finder->getGrid(0,0)->getTotalCol());
	//auto node = grid->getNodeByBase(c, r);
	//node->_walkable = !node->_walkable;
	//if (node->_walkable)
	//	_drawNode->drawSolidRect(Vec2(c, r)*_cellSize, Vec2(c+1, r+1)*_cellSize, Color4F::WHITE);
	//else
	//	_drawNode->drawSolidRect(Vec2(c, r)*_cellSize, Vec2(c + 1, r + 1)*_cellSize, Color4F::GRAY);

	//grid->initClearance();
#endif
}

PathFinderTest::PathFinderTest()
{
	_finder = nullptr;
	_drawNode = nullptr;
	_drawNodeGrid = nullptr;
	
	_debugInfoLabel = nullptr;
	_debugNode = nullptr;

	_reset = nullptr;
	_pathFinding = nullptr;
}

PathFinderTest::~PathFinderTest()
{
	if (_finder)
		delete _finder;
}