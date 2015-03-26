/*
* Frame Buffer Object(FBO):
*  - For render to texture. Such as: scope view, multi pass shader effect, 
*    multi objects render by one pass shader, texture sampling, and so on.
*
* Note:
*  - For now(cocos2dx-3.3final), RenderTexture still have lots of bug. And
*    that's the reason we have to implement a new FBO here!
* 
* XFW, 2014.
*/

#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

#include "cocos2d.h"
#include "DepthStencilTarget.h"
#include "RenderTarget.h"

USING_NS_CC;

/*
* External render texture.
*/
class FrameBuffer: public Ref
{
public:
	static FrameBuffer* create(unsigned int width, unsigned int height);

	bool addRenderTarget(RenderTarget *renderTarget);

	bool setDepthStencilTarget(DepthStencilTarget* target);

	RenderTarget* getRenderTarget(const std::string &id);

	const GLuint getHandle() const;

	FrameBuffer();
	~FrameBuffer();
protected:
	bool init(int width, int height);

private:
	std::string _id;

	GLuint _handle;

	GLuint _currentIdx;

	Vector<RenderTarget*> _renderTargets;

	DepthStencilTarget* _depthStencilTarget;
};
#endif