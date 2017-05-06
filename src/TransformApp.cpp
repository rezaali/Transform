#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TransformApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void TransformApp::setup()
{
}

void TransformApp::mouseDown( MouseEvent event )
{
}

void TransformApp::update()
{
}

void TransformApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( TransformApp, RendererGl )
