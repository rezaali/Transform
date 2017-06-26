#pragma once

#include "cinder/Exception.h"
#include "cinder/Filesystem.h"

#include "Renderer.h"
#include "System.h"

namespace reza {
namespace ps {
typedef std::shared_ptr<class RectRenderer> RectRendererRef;
class RectRenderer : public Renderer {
  public:
	static RectRendererRef create(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		const ci::fs::path &fragmentPath,
		reza::ps::SystemRef systemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr )
	{
		return RectRendererRef( new RectRenderer( window, vertexPath, fragmentPath, systemRef, superFn, glslUpdatedFn, glslErrorFn ) );
	}

	void setupBatch() override;

  protected:
	RectRenderer(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		const ci::fs::path &fragmentPath,
		reza::ps::SystemRef systemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	ci::gl::BatchRef mBatchRef;

	void _draw() override;
};

} // namespace ps
} // namespace reza
