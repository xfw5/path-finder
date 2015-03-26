#include "RenderTextureEx.h"

RenderTextureEx::RenderTextureEx()
:_frameBuffer(nullptr)
, _oldFBO(0)
, _texture(0)
, _clearFlags(0)
, _clearColor(Color4F(0, 0, 0, 0))
, _clearDepth(0.0f)
, _clearStencil(0)
, _sprite(nullptr)
{

}

RenderTextureEx::~RenderTextureEx()
{
	if (_frameBuffer)
		_frameBuffer->release();
}

RenderTextureEx* RenderTextureEx::create(FrameBuffer *frameBuffer)
{
	auto rte = new (std::nothrow) RenderTextureEx();
	if (rte && rte->init(frameBuffer)) {
		rte->autorelease();
		return rte;
	}

	CC_SAFE_DELETE(rte);
	return nullptr;
}

bool RenderTextureEx::init(FrameBuffer *frameBuffer)
{
	if (!frameBuffer) return false;

	_frameBuffer = frameBuffer;
	_frameBuffer->retain();

	_sprite = Sprite::createWithTexture(_frameBuffer->getRenderTarget("1")->getTexture());
	CC_ASSERT(_sprite);
	_sprite->setFlipY(true);
	this->addChild(_sprite);

	return true;
}

void RenderTextureEx::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
	if (!_visible || !isVisitableByVisitingCamera())
	{
		return;
	}

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

	_sprite->visit(renderer, _modelViewTransform, flags);
	draw(renderer, _modelViewTransform, flags);

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void RenderTextureEx::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	begin();

	_clearCommand.init(_globalZOrder);
	_clearCommand.func = CC_CALLBACK_0(RenderTextureEx::onClear, this);
	renderer->addCommand(&_clearCommand);

	sortAllChildren();

	for (const auto &child : _children) {
		if (child != _sprite)
			child->visit(renderer, transform, flags);
	}

	end();
}

void RenderTextureEx::begin()
{
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	_projectionMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_transformMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	_groupCommand.init(_globalZOrder);

	Renderer *renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_groupCommand);
	renderer->pushGroup(_groupCommand.getRenderQueueID());

	_beginCommand.init(_globalZOrder);
	_beginCommand.func = CC_CALLBACK_0(RenderTextureEx::onBegin, this);

	Director::getInstance()->getRenderer()->addCommand(&_beginCommand);
}

void RenderTextureEx::end()
{
	_endCommand.init(_globalZOrder);
	_endCommand.func = CC_CALLBACK_0(RenderTextureEx::onEnd, this);

	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");

	Renderer *renderer = director->getRenderer();
	renderer->addCommand(&_endCommand);
	renderer->popGroup();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void RenderTextureEx::onBegin()
{
	Director *director = Director::getInstance();

	_oldProjMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _projectionMatrix);

	_oldTransMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _transformMatrix);

	director->setProjection(director->getProjection());

	const Size& texSize = _texture->getContentSizeInPixels();

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->getHandle());
}

void RenderTextureEx::onEnd()
{
	Director *director = Director::getInstance();

	glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);

	director->setViewport();

	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _oldProjMatrix);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _oldTransMatrix);

}

void RenderTextureEx::onClear()
{
	// save clear color
	GLfloat oldClearColor[4] = { 0.0f };
	GLfloat oldDepthClearValue = 0.0f;
	GLint oldStencilClearValue = 0;

	// backup and set
	if (_clearFlags & GL_COLOR_BUFFER_BIT)
	{
		glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	}

	if (_clearFlags & GL_DEPTH_BUFFER_BIT)
	{
		glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
		glClearDepth(_clearDepth);
	}

	if (_clearFlags & GL_STENCIL_BUFFER_BIT)
	{
		glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &oldStencilClearValue);
		glClearStencil(_clearStencil);
	}

	// clear
	glClear(_clearFlags);

	// restore
	if (_clearFlags & GL_COLOR_BUFFER_BIT)
	{
		glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
	}
	if (_clearFlags & GL_DEPTH_BUFFER_BIT)
	{
		glClearDepth(oldDepthClearValue);
	}
	if (_clearFlags & GL_STENCIL_BUFFER_BIT)
	{
		glClearStencil(oldStencilClearValue);
	}
}

void RenderTextureEx::onClearDepth()
{
	//! save old depth value
	GLfloat depthClearValue;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);

	glClearDepth(_clearDepth);
	glClear(GL_DEPTH_BUFFER_BIT);

	// restore clear color
	glClearDepth(depthClearValue);
}
