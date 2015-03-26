#include "RenderTarget.h"

RenderTarget::RenderTarget()
{
	_id = "";
	_texture = nullptr;
}

RenderTarget::~RenderTarget()
{
	if (_texture)
		_texture->release();

	_id = "";
}

RenderTarget* RenderTarget::create(const std::string &id, unsigned int width, unsigned int height)
{
	auto texture = new (std::nothrow) Texture2D();
	if (texture) {
		auto dataLen = width * height * 4;
		auto data = malloc(dataLen);
		if (!data) return nullptr;

		texture->initWithData(data, dataLen, Texture2D::PixelFormat::RGBA8888, \
			width, height, Size((float)width, (float)height));
	}

	RenderTarget* rt = create(id, texture);
	texture->release();

	return rt;
}

RenderTarget* RenderTarget::create(const std::string &id, Texture2D* texture)
{
	RenderTarget* rt = new RenderTarget();

	if (rt && rt->init(id, texture)) {
		rt->autorelease();
		return rt;
	}

	CC_SAFE_DELETE(rt);
	return nullptr;
}

bool RenderTarget::init(const std::string &id, Texture2D* texture)
{
	_id = id;

	_texture = texture;
	_texture->retain();

	return true;
}

Texture2D* RenderTarget::getTexture() const
{
	return _texture;
}

std::string& RenderTarget::getId()
{
	return _id;
}
