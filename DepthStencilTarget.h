/*
* Depth and stencil render target for FBO render.
*
* XFW, 2015.
*/

#ifndef _DEPTH_STENCIL_TARGET_H__
#define _DEPTH_STENCIL_TARGET_H__

#include "cocos2d.h"

USING_NS_CC;

class DepthStencilTarget: public Ref
{
public:

	enum DSType {
		DEPTH = 0, 
		DEPTH_STENCIL
	};

	static DepthStencilTarget* create(const char* id, DSType type, unsigned int width, unsigned int height);

	GLuint getDepthBufferHandle();
	GLuint getStencilBufferHandle();

	bool isPacked();

	DepthStencilTarget();
	~DepthStencilTarget();
protected:
	bool init(const char* id, DSType type, unsigned int width, unsigned int height);

private:
	std::string _id;

	DSType _type;
	
	GLuint _depthBuffer;
	GLuint _stencilBuffer;

	unsigned int _width;
	unsigned int _height;

	bool _packed;
};
#endif