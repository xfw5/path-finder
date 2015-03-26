#include "FrameBuffer.h"
#include "RenderTarget.h"

FrameBuffer::FrameBuffer()
:_id("")
, _handle(0)
, _currentIdx(0)
, _depthStencilTarget(nullptr)
{

}

FrameBuffer::~FrameBuffer()
{
	_renderTargets.clear();

	if (_depthStencilTarget)
		_depthStencilTarget->release();

	if (_handle) {
		glDeleteFramebuffers(1, &_handle);
		CHECK_GL_ERROR_DEBUG();
	}
}

FrameBuffer* FrameBuffer::create(unsigned int width, unsigned int height)
{
	auto fb = new (std::nothrow) FrameBuffer();
	if (fb && fb->init(width, height)) {
		fb->autorelease();
		return fb;
	}

	CC_SAFE_DELETE(fb);
	return nullptr;
}

bool FrameBuffer::init(int width, int height)
{
	_handle = 0;

	glGenFramebuffers(1, &_handle);
	CHECK_GL_ERROR_DEBUG();

	return true;
}

bool FrameBuffer::addRenderTarget(RenderTarget *renderTarget)
{
	if (!renderTarget) return false;

	GLint oldHandle;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldHandle);
	CHECK_GL_ERROR_DEBUG();

	glBindFramebuffer(GL_FRAMEBUFFER, _handle);
	CHECK_GL_ERROR_DEBUG();

	GLenum attachment = GL_COLOR_ATTACHMENT0 + _currentIdx;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, renderTarget->getTexture()->getName(), 0);
	CHECK_GL_ERROR_DEBUG();

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		CCLOGERROR("Framebuffer status incomplete: 0x%x", fboStatus);
		return false;
	}

	_renderTargets.pushBack(renderTarget);

	glBindFramebuffer(GL_FRAMEBUFFER, oldHandle);
	CHECK_GL_ERROR_DEBUG();

	return true;
}

bool FrameBuffer::setDepthStencilTarget(DepthStencilTarget* target)
{
	if (!target || _depthStencilTarget == target) return false;

	CC_SAFE_RELEASE(_depthStencilTarget);
	_depthStencilTarget = target;
	CC_SAFE_RETAIN(_depthStencilTarget);

	GLint oldHandle;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldHandle);
	CHECK_GL_ERROR_DEBUG();

	glBindFramebuffer(GL_FRAMEBUFFER, _handle);
	CHECK_GL_ERROR_DEBUG();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, \
		GL_RENDERBUFFER, _depthStencilTarget->getDepthBufferHandle());
	CHECK_GL_ERROR_DEBUG();

	if (target->isPacked()) {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, \
			GL_RENDERBUFFER, _depthStencilTarget->getDepthBufferHandle());
		CHECK_GL_ERROR_DEBUG();
	}

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		CCLOGERROR("Framebuffer status incomplete: 0x%x", fboStatus);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, oldHandle);
	CHECK_GL_ERROR_DEBUG();

	return true;
}

RenderTarget* FrameBuffer::getRenderTarget(const std::string &id)
{
	for (auto it : _renderTargets) {
		if (it->getId().compare(id)) continue;

		return it;
	}

	return nullptr;
}

const GLuint FrameBuffer::getHandle() const
{
	return _handle;
}
