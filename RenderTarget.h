/*
* Render target:
*  - Cause of cocos2dx engine only support texture 2D,
*    render texture don't support texture cube.
*
* XFW, 2015.
*/

#ifndef __RENDER_TARGET_H__
#define __RENDER_TARGET_H__

#include "cocos2d.h"

USING_NS_CC;

class RenderTarget: public Ref
{
public:
	static RenderTarget* create(const std::string &id, unsigned int width, unsigned int height);

	static RenderTarget* create(const std::string &id, Texture2D* texture);

	Texture2D* getTexture() const;

	std::string& getId();

protected:
	bool init(const std::string &id, Texture2D* texture);

private:
	RenderTarget();
	~RenderTarget();

	std::string _id;
	Texture2D* _texture;
};

#endif