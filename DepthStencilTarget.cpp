#include "DepthStencilTarget.h"
#include "base\ccMacros.h"

DepthStencilTarget::DepthStencilTarget()
:_id("")
, _packed(false)
, _depthBuffer(0)
, _stencilBuffer(0)
, _width(0)
, _height(0)
{

}

DepthStencilTarget::~DepthStencilTarget()
{
	if (_depthBuffer) {
		glDeleteRenderbuffers(1, &_depthBuffer);
		CHECK_GL_ERROR_DEBUG();
	}

	if (_stencilBuffer) {
		glDeleteRenderbuffers(1, &_stencilBuffer);
		CHECK_GL_ERROR_DEBUG();
	}
}

DepthStencilTarget* DepthStencilTarget::create(const char* id, DSType type, unsigned int width, unsigned int height)
{
	auto* dst = new DepthStencilTarget();
	if (dst && dst->init(id, type, width, height)) {
		dst->autorelease();
		return dst;
	}

	CC_SAFE_DELETE(dst);
	return nullptr;
}

bool DepthStencilTarget::init(const char* id, DSType type, unsigned int width, unsigned int height)
{
	GLint oldRBO;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);

	glGenRenderbuffers(1, &_depthBuffer);
	CHECK_GL_ERROR_DEBUG();

	glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
	CHECK_GL_ERROR_DEBUG();

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	CHECK_GL_ERROR_DEBUG();

	glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
	CHECK_GL_ERROR_DEBUG();

	_id = id;
	_width = width;
	_height = height;

	_packed = true;

	return true;
}

GLuint DepthStencilTarget::getDepthBufferHandle()
{
	return _depthBuffer;
}

GLuint DepthStencilTarget::getStencilBufferHandle()
{
	return _stencilBuffer;
}

bool DepthStencilTarget::isPacked()
{
	return _packed;
}
