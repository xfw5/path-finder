#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

#include "cocos2d.h"
#include "FrameBuffer.h"

USING_NS_CC;

class RenderTextureEx: public Node
{
public:
	static RenderTextureEx* create(FrameBuffer *frameBuffer);

	void onDraw(const Mat4 &transform, uint32_t flags);

	void visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;

	void begin();
	void end();

	void setPosition(const Vec2 &position) { _sprite->setPosition(position); }

	RenderTextureEx();
	~RenderTextureEx();

protected:
	bool init(FrameBuffer *frameBuffer);

	void onBegin();
	void onEnd();

	void onClear();
	void onClearDepth();
private:

	Sprite* _sprite;

	FrameBuffer *_frameBuffer;
	Texture2D *_texture;

	GroupCommand _groupCommand;

	CustomCommand _beginWithClearCommand;
	CustomCommand _clearDepthCommand;
	CustomCommand _clearCommand;
	CustomCommand _beginCommand;
	CustomCommand _endCommand;

	GLint        _oldFBO;

	Mat4 _oldTransMatrix, _oldProjMatrix;
	Mat4 _transformMatrix, _projectionMatrix;

	GLbitfield   _clearFlags;
	Color4F    _clearColor;
	GLclampf     _clearDepth;
	GLint        _clearStencil;
};

#endif