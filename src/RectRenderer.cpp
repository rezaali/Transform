#include "..\include\RectRenderer.h"

#include "LiveCode.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

RectRenderer::RectRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	reza::ps::ParticleSystemRef &particleSystemRef,
	std::function<void()> superFn,
	std::function<void( GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: Renderer( window, vertexPath, fragmentPath, static_cast<SystemRef>( particleSystemRef ), superFn, glslUpdatedFn, glslErrorFn )
{
}

void RectRenderer::setupBatch()
{
	if( mGlslProgRef ) {
		mBatchRef = gl::Batch::create(
			geom::Rect( Rectf( 0.0f, 0.0f, float( mWindowRef->getWidth() ), float( mWindowRef->getHeight() ) ) ),
			mGlslProgRef );
		Renderer::setupBatch();
	}
}

void RectRenderer::_draw()
{
	if( mBatchRef ) {
		mGlslParamsRef->applyUniforms( mGlslProgRef );
		mBatchRef->draw();
	}
}

} // namespace ps
} // namespace reza
