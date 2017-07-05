// CINDER
#include "cinder/CameraUi.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/audio.h"

// STD
#include <chrono>
#include <regex>

// RESOURCES
#include "Resources.h"

// BLOCKS
#include "AppUI.h"
#include "EasyCamera.h"
#include "Globals.h"
#include "GlslParams.h"
#include "ImageSaver.h"
#include "LiveCode.h"
#include "MovieSaver.h"
#include "Paths.h"
#include "SaveLoadCamera.h"
#include "SaveLoadWindow.h"
#include "SequenceSaver.h"
#include "Tiler.h"
#include "UI.h"
#include "Watchdog.h"

// SYSTEMS
#include "ParticleSystem.h"
#include "PlexusSystem.h"
#include "TrailSystem.h"

// RENDERERS
#include "PlexusRenderer.h"
#include "PointRenderer.h"
#include "PrimitiveRenderer.h"
#include "RibbonRenderer.h"
#include "SpriteRenderer.h"
#include "TrailPointRenderer.h"

#include "RectRenderer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace reza::win;
using namespace reza::ui;
using namespace reza::paths;
using namespace reza::cam;
using namespace reza::glsl;
using namespace reza::tiler;
using namespace reza::cam;
using namespace reza::app;
using namespace reza::img;
using namespace reza::mov;
using namespace reza::seq;
using namespace reza::ps;

/*
TODO:

+ Integrate Color Palettes
+ Extend Primitive Renderer
+ Camera Animtion UI (Render to a 1 pixel fbo? pos, orientation quat)
+ Implement OSC for Params Control
+ Export Points (ply, draco, convex hull, poisson recreation)
+ Export Curves (MEL)
+ Export Meshes Spheres (OpenVDB)

DONE:
+ Fix Loading Multiple Times Issues
+ Get this working on OSX [FUCK WINDOWS]
+ Render Plane in Background
+ Create Post Processing Pass
+ Create Audio UI Menu
+ Create APP UI
+ Fix UI Menu Disappearing Issue
+ Render Everything to an FBO
+ Create Primitive Renderer
+ Save & Loading For Shaders & Settings
+ Integrate Audio Amplitude & FFT Samples
+ Create GLSLParams for Render Geom Shader
+ Create GLSLParams for Render Trail Shader
+ Create RibbonRenderer
+ Create TrailPointRenderer
+ Create GLSLParams for Render Plexus Shader
+ Create Plexus Renderer
+ Refactor Sprites Renderer
+ Refactor PS System to use System Class for Future TF
+ Refactor PS to TFPS Class
+ Refactor Record Sequence Code
+ Create GLSLParams for Render Sprite Shader
+ Create GLSLParams for Render Points Shader
+ Implement Particles UI
+ Refactor Save Image
+ Refactor UI Code to Make it Reusable
+ Refactor Camera Code to Make it Reusable
+ Implement Image Sequence Rendering
+ Setup ShaderToy Uniform Inputs
+ Figure Out shader saving approach
+ Implement Tile Rendering
+ Control Background Color
+ Implement Save & Load Window
+ Setup Basic Transform Feedback Particle System (TFPS)
+ Setup Basic Live Reloading for TFPS
+ Create UI for PS Params
+ Setup GlslParams for TFPS
+ Implement Save & Loading for Live coding Sessions
+ Create Directories Procedurally
+ Create OutputWindowRef
+ Save & Load
+ 3D Camera System
+ Draw Debug Area
*/

class Transform : public App {
  public:
	// PREPARE SETTINGS
	static void prepareSettings( Settings *settings );

	// SETUP
	void setup() override;

	// CLEANUP
	void cleanup() override;

	// FS
	void createAssetDirectories();

	// SAVE & LOAD
	void save();
	void load();

	void saveSession();
	void loadSession();

	void saveAs( const fs::path &path );
	void saveSettings( const fs::path &path );
	void saveShaders( const fs::path &path );

	void copyShaders( const fs::path &fromPath, const fs::path &toPath );
	void copyFolder( const fs::path &fromPath, const fs::path &toPath );

	void loadAs( const fs::path &path );
	void loadSettings( const fs::path &path );
	void loadShaders( const fs::path &path );

	// OUTPUT
	gl::FboRef mOutputFboRef;
	void setupOutputFbo();
	void setupOutputWindow();
	void updateOutput();
	void drawOutput();
	void _drawOutput();
	void keyDownOutput( KeyEvent event );
	ci::app::WindowRef mOutputWindowRef;
	vec2 mMouse = vec2( 0.0 );
	vec2 mMousePrev = vec2( 0.0 );
	vec2 mMouseClick = vec2( 0.0 );
	ivec2 mOutputWindowOrigin = ivec2( 0 );
	ivec2 mOutputWindowSize = ivec2( 1920, 1080 );

	// UI
	AppUIRef mUIRef;
	void setupUIs();
	UIPanelRef setupAppUI( UIPanelRef ui );
	UIPanelRef setupAudioUI( UIPanelRef ui );
	UIPanelRef setupPostUI( UIPanelRef ui );
	UIPanelRef setupBgUI( UIPanelRef ui );
	UIPanelRef setupSystemUI( UIPanelRef ui, reza::ps::SystemRef systemRef );
	UIPanelRef setupPhysicsUI( UIPanelRef ui );
	UIPanelRef setupTrailUI( UIPanelRef ui );
	UIPanelRef setupPlexusUI( UIPanelRef ui );
	UIPanelRef setupExporterUI( UIPanelRef ui );
	UIPanelRef setupCameraUI( UIPanelRef ui );
	UIPanelRef setupRendererUI( UIPanelRef ui, reza::ps::RendererRef rendererRef );

	// COLORS
	ci::gl::Texture2dRef mPaletteTextureRef;
	void loadPalettes();
	void setupPalettes();

	// CAMERA
	EasyCameraRef mCameraRef;

	// BACKGROUND
	ColorA mBgColor = ColorA::black();

	// BOX
	bool mDrawBox = true;
	gl::BatchRef mBoxBatchRef;
	vec3 mBoxSize = vec3( 5.0f, 5.0f, 5.0f );
	ColorA mBoxColor = ColorA::white();
	void setupBoxBatch();
	void drawBoxBatch();

	// GRID
	int mGridPointSize = 2;
	int mGridSubdivisons = 20;
	gl::BatchRef mGridLinesBatchRef;
	gl::BatchRef mGridPointsBatchRef;
	bool mDrawGridLines = true;
	bool mDrawGridPoints = true;
	ColorA mGridLinesColor = ColorA::white();
	ColorA mGridPointsColor = ColorA::white();
	void setupGrid();
	void drawGridLines();
	void drawGridPoints();

	vector<reza::ps::RendererRef> mRenderers;
	vector<reza::ps::SystemRef> mSystems;

	void updateSystems();
	void updateRenderers();
	void drawRenderers();

	// PARTICLE SYSTEM
	ParticleSystemRef mParticleSystemRef;
	void setupParticles();

	// PLEXUS SYSTEM
	PlexusSystemRef mPlexusSystemRef;
	void setupPlexus();

	// TRAIL SYSTEM
	TrailSystemRef mTrailSystemRef;
	void setupTrail();

	// POINT RENDERER
	PointRendererRef mPointRendererRef;
	void setupPoints();

	// SPRITES RENDERER
	PrimitiveRendererRef mSpriteRendererRef;
	void setupSprites();

	// PLEXUS RENDERER
	PlexusRendererRef mPlexusRendererRef;
	void setupLines();

	// TRAIL POINTS RENDERER
	TrailPointRendererRef mTrailPointRendererRef;
	void setupTrailPoint();

	// RIBBON RENDERER
	RibbonRendererRef mRibbonRendererRef;
	void setupRibbon();

	// PRIMTIVE RENDERER
	PrimitiveRendererRef mPrimitiveRendererRef;
	void setupPrimitive();

	// SHADER TOYIFIER
	void setDefaultUniforms( gl::GlslProgRef glslProgRef );

	// IMAGE EXPORTER
	ImageSaverRef mImageSaverRef;
	void setupImageSaver();

	// MOVIE EXPORTER
	MovieSaverRef mMovieSaverRef;
	void setupMovieSaver();

	// SEQUENCE EXPORTER
	SequenceSaverRef mSequenceSaverRef;
	void setupSequenceSaver();
	bool mSaveMovie = false;
	bool mSaveSequence = false;
	int mTotalFrames = 120;
	float mCurrentTime = 0.0f;
	float mSeconds = 0.0;

	// SAVING & LOADING
	ci::fs::path mDefaultRenderPath;
	fs::path mDefaultSaveLoadPath;
	fs::path mDefaultMoviePath;

	// AUDIO
	audio::InputDeviceNodeRef mInputDeviceNode;
	audio::MonitorSpectralNodeRef mMonitorSpectralNode;
	void setupAudio();
	void setupAudioSpectrumTexture();
	void setupAudioAmplitudeTexture();
	void setupAudioInput();
	void updateAudio();
	vector<float> mSpectrumBuffer;
	vector<float> mAmplitudeBuffer;

	ci::gl::Texture2dRef mSpectrumTextureRef;
	ci::gl::Texture2dRef mAmplitudeTextureRef;

	// POST
	RectRendererRef mRectRendererRef;
	void setupPost();
	void updatePost();
	void drawPost();
	void drawPost( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll );

	// BACKGROUND
	RectRendererRef mBgRendererRef;
	void setupBg();
	void updateBg();
	void drawBg();
	void drawBg( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll );
};

void Transform::prepareSettings( App::Settings *settings )
{
	settings->setWindowSize( 512, 512 );
	settings->setFrameRate( 60.0f );
	settings->setHighDensityDisplayEnabled();
}

void Transform::setup()
{
	createAssetDirectories();
	setupOutputWindow();
	setupOutputFbo();

	// PALETTES
	setupPalettes();

	// CAMERA
	mCameraRef = EasyCamera::create( mOutputWindowRef );
	mCameraRef->enable();

	// SAVERS
	setupImageSaver();
	setupSequenceSaver();
	setupMovieSaver();

	// DEBUG
	setupBoxBatch();
	setupGrid();

	// SYSTEMS
	setupParticles();
	setupPlexus();
	setupTrail();

	// RENDERERS
	setupPoints();
	setupSprites();
	setupLines();
	setupTrailPoint();
	setupRibbon();
	setupPrimitive();
	setupPost();
	setupBg();

	// AUDIO
	setupAudio();

	// UI
	setupUIs();

	// SETTINGS
	loadSettings( getPath() );
}

void Transform::cleanup()
{
	saveSettings( getPath() );
}

void Transform::createAssetDirectories()
{
	createDirectories( getSettingsPath() );
	createDirectories( getShadersPath() );
}

void Transform::save()
{
	saveAs( getPath() );
}

void Transform::saveAs( const fs::path &path )
{
	createDirectories( path );
	saveSettings( path );
	saveShaders( path );
}

void Transform::saveSettings( const fs::path &path )
{
	auto settingsPath = addPath( path, SETTINGS_PATH );
	saveWindow( addPath( settingsPath, WINDOW_PATH ), mOutputWindowOrigin, mOutputWindowSize, true );
	saveCamera( addPath( settingsPath, CAMERA_PATH ), mCameraRef->getCameraPersp() );
	mUIRef->saveUIs( settingsPath );
}

void Transform::saveShaders( const fs::path &path )
{
	auto shadersPath = addPath( path, SHADERS_PATH );
	createDirectory( shadersPath );
	copyShaders( getShadersPath(), shadersPath );
}

void Transform::copyShaders( const fs::path &fromPath, const fs::path &toPath )
{
	map<fs::path, fs::path> directories;
	directories[fs::path( "" )] = fromPath;

	fs::recursive_directory_iterator it( fromPath ), eit;
	for( ; it != eit; ++it ) {
		fs::path pth = it->path();
		if( fs::is_directory( pth ) ) {
			directories[pth.filename()] = pth;
		}
	}

	fs::path folder = toPath;
	createDirectory( folder );

	for( auto &it : directories ) {
		fs::path dir = folder;
		if( !it.first.empty() ) {
			dir += "/";
			dir += it.first;
			createDirectory( dir );
		}

		fs::directory_iterator dit( it.second ), deit;
		for( ; dit != deit; ++dit ) {
			fs::path from = dit->path();
			if( !fs::is_directory( from ) ) {
				if( from.extension() != ".DS_Store" ) {
					fs::path to = dir;
					to += "/";
					to += from.filename();
					if( fs::exists( to ) ) {
						fs::remove( to );
					}
					fs::copy( from, to );
				}
			}
		}
	}
}

void Transform::copyFolder( const fs::path &fromPath, const fs::path &toPath )
{
	map<fs::path, fs::path> directories;
	directories[fs::path( "" )] = fromPath;

	fs::recursive_directory_iterator it( fromPath ), eit;
	for( ; it != eit; ++it ) {
		fs::path pth = it->path();
		if( fs::is_directory( pth ) ) {
			directories[pth.filename()] = pth;
		}
	}

	fs::path folder = toPath;
	createDirectory( folder );

	for( auto &it : directories ) {
		fs::path dir = folder;
		if( !it.first.empty() ) {
			dir += "/";
			dir += it.first;
			createDirectory( dir );
		}

		fs::directory_iterator dit( it.second ), deit;
		for( ; dit != deit; ++dit ) {
			fs::path from = dit->path();
			if( !fs::is_directory( from ) ) {
				if( from.extension() != ".DS_Store" ) {
					fs::path to = dir;
					to += "/";
					to += from.filename();
					if( fs::exists( to ) ) {
						fs::remove( to );
					}
					fs::copy( from, to );
				}
			}
		}
	}
}

void Transform::load()
{
	loadAs( getPath() );
}

void Transform::loadAs( const fs::path &path )
{
	loadSettings( path );
	loadShaders( path );
}

void Transform::loadSettings( const fs::path &path )
{
	auto settingsPath = addPath( path, SETTINGS_PATH );
	loadWindow( addPath( settingsPath, WINDOW_PATH ), false, [this]( ivec2 origin, ivec2 size, bool closed ) {
		mOutputWindowRef->setPos( origin );
		mOutputWindowRef->setSize( size );
	} );
	loadCamera( addPath( settingsPath, CAMERA_PATH ), mCameraRef->getCameraPersp(), [this]() { mCameraRef->update(); } );
	mUIRef->loadUIs( settingsPath );
}

void Transform::loadShaders( const fs::path &path )
{
	auto shadersPath = addPath( path, SHADERS_PATH );
	copyShaders( shadersPath, getShadersPath() );
}

void Transform::setupOutputWindow()
{
	mOutputWindowRef = getWindow();
	mOutputWindowRef->getSignalClose().connect( [this] { quit(); } );
	mOutputWindowRef->getSignalDraw().connect( [this] {
		updateOutput();
		mImageSaverRef->update();
		mSequenceSaverRef->update();
		drawOutput();
		mMovieSaverRef->update();
	} );
	mOutputWindowRef->getSignalResize().connect( [this] {
		mOutputWindowSize = mOutputWindowRef->getSize();
		saveCamera( getSettingsPath( CAMERA_PATH ), mCameraRef->getCameraPersp() );
		mCameraRef->setup();
		loadCamera( getSettingsPath( CAMERA_PATH ), mCameraRef->getCameraPersp(), [this]() { mCameraRef->update(); } );
		setupOutputFbo();
		if( mRectRendererRef ) {
			mRectRendererRef->setupBatch();
		}
		if( mBgRendererRef ) {
			mBgRendererRef->setupBatch();
		}
	} );
	mOutputWindowRef->getSignalMove().connect(
		[this] { mOutputWindowOrigin = mOutputWindowRef->getPos(); } );
	mOutputWindowRef->getSignalKeyDown().connect(
		[this]( KeyEvent event ) { keyDownOutput( event ); } );
	mOutputWindowRef->getSignalMouseDown().connect( [this]( MouseEvent event ) {
		if( mUIRef ) {
			if( mUIRef->isHit( event.getPos() ) ) {
				if( mCameraRef ) {
					mCameraRef->disable();
				}
			}
		}
		mMousePrev = mMouseClick = mMouse = vec2( event.getPos() );
	} );
	mOutputWindowRef->getSignalMouseDrag().connect( [this]( MouseEvent event ) {
		mMousePrev = mMouse;
		mMouse = event.getPos();
	} );
	mOutputWindowRef->getSignalMouseUp().connect( [this]( MouseEvent event ) {
		mMouse = vec2( 0.0 );
		if( mCameraRef ) {
			mCameraRef->enable();
		}
	} );
}

void Transform::setupOutputFbo()
{
	gl::Fbo::Format fmt;
	//	fmt.samples( gl::Fbo::getMaxSamples() );
	mOutputFboRef = gl::Fbo::create( toPixels( mOutputWindowRef->getWidth() ),
		toPixels( mOutputWindowRef->getHeight() ),
		fmt );
}

void Transform::updateOutput()
{
	mSeconds += 0.016f;
	mOutputWindowRef->setTitle( to_string( (int)getAverageFps() ) + " FPS" );
	mOutputWindowRef->getRenderer()->makeCurrentContext();

	if( mSequenceSaverRef->isRecording() ) {
		mCurrentTime = mSequenceSaverRef->getCurrentTime();
	}
	else if( mMovieSaverRef->isRecording() ) {
		mCurrentTime = mMovieSaverRef->getCurrentTime();
	}
	else {
		mCurrentTime = mSequenceSaverRef->getCurrentTime();
	}

	updateAudio();
	updateSystems();
	updateRenderers();
	updateBg();
	updatePost();
}

void Transform::drawOutput()
{
	gl::clear( mBgColor );
	if( mOutputFboRef ) {
		mOutputFboRef->bindFramebuffer();
		gl::ScopedViewport scpVp( ivec2( 0 ), mOutputFboRef->getSize() );
		drawBg();
		gl::setMatrices( mCameraRef->getCameraPersp() );
		_drawOutput();
		mOutputFboRef->unbindFramebuffer();
		drawPost();
	}
}

void Transform::_drawOutput()
{
	drawBoxBatch();
	drawGridLines();
	drawGridPoints();
	drawRenderers();
}

void Transform::keyDownOutput( KeyEvent event )
{
	if( event.isControlDown() ) {
		switch( event.getCode() ) {
		case KeyEvent::KEY_p: {
			mUIRef->toggleUIs();
		} break;
		case KeyEvent::KEY_f: {
			mOutputWindowRef->setFullScreen( !mOutputWindowRef->isFullScreen() );
			setupOutputFbo();
		} break;
		case KeyEvent::KEY_LEFTBRACKET: {
			mUIRef->minimize();
			mUIRef->arrange();
		} break;
		case KeyEvent::KEY_RIGHTBRACKET: {
			mUIRef->maximize();
			mUIRef->arrange();
		} break;
		case KeyEvent::KEY_SLASH: {
			mUIRef->arrange();
		} break;

		case KeyEvent::KEY_c: {
			hideCursor();
		} break;

		case KeyEvent::KEY_x: {
			showCursor();
		} break;
		}
	}
}

void Transform::setupUIs()
{
	mUIRef = AppUI::create();

	// APP
	mUIRef->setupUI(
		APP_UI, [this]( UIPanelRef ui ) -> UIPanelRef { return setupAppUI( ui ); } );
	mUIRef->setupUI( AUDIO_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupAudioUI( ui );
	} );
	mUIRef->setupUI( BACKGROUND_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupBgUI( ui );
	} );
	mUIRef->setupUI(
		POST_UI, [this]( UIPanelRef ui ) -> UIPanelRef { return setupPostUI( ui ); } );
	mUIRef->setupUI( CAMERA_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupCameraUI( ui );
	} );
	mUIRef->setupUI( EXPORTER_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupExporterUI( ui );
	} );

	// SYSTEMS
	mUIRef->setupUI( PHYSICS_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupPhysicsUI( ui );
	} );
	mUIRef->setupUI( PLEXUS_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupPlexusUI( ui );
	} );
	mUIRef->setupUI( TRAIL_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupTrailUI( ui );
	} );

	// RENDERERS
	mUIRef->setupUI( SPRITES_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mSpriteRendererRef );
	} );
	mUIRef->setupUI( POINTS_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mPointRendererRef );
	} );
	mUIRef->setupUI( LINES_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mPlexusRendererRef );
	} );
	mUIRef->setupUI( TRAIL_POINTS_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mTrailPointRendererRef );
	} );
	mUIRef->setupUI( RIBBON_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mRibbonRendererRef );
	} );
	mUIRef->setupUI( PRIMITIVE_UI, [this]( UIPanelRef ui ) -> UIPanelRef {
		return setupRendererUI( ui, mPrimitiveRendererRef );
	} );
}

void Transform::saveSession()
{
	auto rtrim = []( string input, string key ) {
		string result = "";
		size_t found = input.rfind( key );
		if( found != string::npos ) {
			result = input.substr( 0, found );
		}
		return result;
	};

	auto prePath = mDefaultSaveLoadPath;
	auto pth = getSaveFilePath( prePath );
	if( !pth.empty() ) {
		if( createDirectory( pth ) ) {
			mDefaultSaveLoadPath = fs::path( rtrim( pth.string(), "/" ) );
			saveAs( pth );
		}
	}
}

void Transform::loadSession()
{
	auto pth = getFolderPath( mDefaultSaveLoadPath );
	if( !pth.empty() ) {
		copyFolder( addPath( pth, "Settings" ), getSettingsPath() );
		copyFolder( addPath( pth, "Shaders" ), getShadersPath() );

		for( auto &it : mSystems ) {
			it->setupGlsl();
			it->setInitialized( false );
		}

		for( auto &it : mRenderers ) {
			it->setupGlsl();
			it->setInitialized( false );
		}

		mDefaultSaveLoadPath = pth.parent_path();
	}
}

UIPanelRef Transform::setupAppUI( UIPanelRef ui )
{
	ui->clear();
	ui->addSpacer();
	ui->addButton( "SAVE SESSION", false )->setCallback( [this]( bool value ) {
		if( value ) {
			saveSession();
		}
	} );

	ui->addButton( "LOAD SESSION", false )->setCallback( [this]( bool value ) {
		if( value ) {
			loadSession();
		}
	} );

	ui->addSpacer();
	auto clrfmt = ColorPicker::Format().label( false );
	ui->addColorPicker( "BACKGROUND COLOR", &mBgColor );
	ui->addColorPicker( "BOX COLOR", &mBoxColor, clrfmt );
	ui->right();
	ui->addToggle( "DRAW BOX", &mDrawBox );
	ui->down();
	ui->addSpacer();
	ui->addColorPicker( "GRID LINES COLOR", &mGridLinesColor, clrfmt );
	ui->right();
	ui->addToggle( "DRAW GRID LINES", &mDrawGridLines );
	ui->down();
	ui->addColorPicker( "GRID POINTS COLOR", &mGridPointsColor, clrfmt );
	ui->right();
	ui->addToggle( "DRAW GRID POINTS", &mDrawGridPoints );
	ui->down();
	ui->addSpacer();
	return ui;
}

UIPanelRef Transform::setupAudioUI( UIPanelRef ui )
{
	ui->clear();
	ui->addSpacer();

	map<string, string> shortToLongMap;
	vector<string> names;
	auto devices = audio::Device::getInputDevices();
	for( auto &it : devices ) {
		string name = it->getName();
		if( name.length() > 35 ) {
			string shortName = name.substr( 0, 35 );
			names.push_back( shortName );
			shortToLongMap[shortName] = name;
		}
		else {
			names.push_back( name );
			shortToLongMap[name] = name;
		}
	}

	auto rfmt = Radio::Format().direction( Direction::SOUTH );

	auto input = ui->addRadio( "INPUT", names, rfmt );
	input->setCallback( [this, shortToLongMap]( string name, bool value ) {
		if( value ) {
			auto ctx = audio::Context::master();
			auto it = shortToLongMap.find( name );
			if( it != shortToLongMap.end() ) {
				CI_LOG_D( "SWITCHING AUDIO DEVICE: " + name );
				audio::DeviceRef device = audio::Device::findDeviceByName( it->second );
				mInputDeviceNode = ctx->createInputDeviceNode( device );
				mInputDeviceNode >> mMonitorSpectralNode;
				mInputDeviceNode->enable();
				ctx->enable();
			}
		}
	} );

	ui->addSpacer();
	ui->addTexture( "AMPLITUDE", mAmplitudeTextureRef, TextureView::Format().height( 32 ) );
	ui->addTexture( "SPECTRUM", mSpectrumTextureRef, TextureView::Format().height( 32 ) );
	return ui;
}

UIPanelRef Transform::setupPostUI( UIPanelRef ui )
{
	return ui;
}

UIPanelRef Transform::setupBgUI( UIPanelRef ui )
{
	return ui;
}

UIPanelRef Transform::setupCameraUI( UIPanelRef ui )
{
	ui->clear();
	ui->addSpacer();
	ui->addButton( "RESET CAMERA", false )->setCallback( [this]( bool value ) {
		if( value ) {
			mCameraRef->setup();
		}
	} );
	ui->addSliderf( "FOV", &mCameraRef->getFov(), 0.0f, 180.0f )
		->setCallback( [this]( float value ) { mCameraRef->update(); } );

	return ui;
}

UIPanelRef Transform::setupExporterUI( UIPanelRef ui )
{
	ui->clear();
	ui->addSpacer();
	ui->addButton( "SAVE IMAGE AS", false )->setCallback( [this]( bool value ) {
		if( value ) {
			fs::path path = ci::app::getSaveFilePath( mDefaultRenderPath );
			if( !path.empty() ) {
				mDefaultRenderPath = path.parent_path();
				string filename = path.filename().string();
				string ext = path.extension().string();
				string dir = path.parent_path().string() + "/";

				fs::path opath = fs::path( dir );
				auto it = filename.find( "." );
				if( it != string::npos ) {
					filename = filename.substr( 0, it );
				}
				vector<string> extensions = { "png", "jpg", "tif" };
				bool valid = false;
				for( auto it : extensions ) {
					if( it == ext ) {
						valid = true;
						break;
					}
				}
				if( !valid ) {
					ext = "png";
				}
				dispatchSync( [this, opath, filename, ext]() {
					saveAs( addPath( opath, filename ) );
					mImageSaverRef->save( mCameraRef->getCameraPersp(), opath, filename, ext );
				} );
			}
		}
	} );
	ui->down();
	ui->addDialeri( "OUTPUT IMAGE SCALE", mImageSaverRef->getSizeMultiplier(), 1, 20 )
		->setCallback( [this]( int value ) {
			mImageSaverRef->setSizeMultiplier( value );
			mSequenceSaverRef->setSizeMultiplier( value );
		} );

	ui->addSpacer();
	ui->addButton( "RENDER", false )->setCallback( [this]( bool value ) {
		if( value && ( mSaveMovie || mSaveSequence ) ) {
			fs::path path = getSaveFilePath( mDefaultMoviePath );
			if( !path.empty() ) {
				mDefaultMoviePath = path.parent_path();

				string filename = path.filename().string();
				string dir = path.parent_path().string() + "/";
				fs::path opath = fs::path( dir );

				auto it = filename.rfind( "." );
				if( it != string::npos ) {
					filename = filename.substr( 0, it );
				}

				saveAs( addPath( opath, filename ) );
				if( mSaveMovie ) {
					mMovieSaverRef->save( opath, filename, "mov" );
				}

				if( mSaveSequence ) {
					mSequenceSaverRef->save( mCameraRef->getCameraPersp(),
						addPath( opath, filename ),
						filename,
						"png" );
				}
			}
		}
	} );
	ui->right();
	ui->addToggle( "MOV", &mSaveMovie );
	ui->addToggle( "PNG", &mSaveSequence );
	ui->addDialeri( "FRAMES", &mTotalFrames, 0, 99999, Dialeri::Format().label( false ) )
		->setCallback( [this]( int value ) {
			mMovieSaverRef->setTotalFrames( value );
			mSequenceSaverRef->setTotalFrames( value );
		} );
	ui->down();
	ui->setSliderHeight( 8 );
	ui->addSliderf( "PROGRESS", &mCurrentTime, 0.0, 1.0, Sliderf::Format().label( false ) );
	return ui;
}

UIPanelRef Transform::setupRendererUI( UIPanelRef ui,
	reza::ps::RendererRef rendererRef )
{
#if USE_WINDOW_CANVAS
#else
	ui->addSpacer();
#endif
	ui->addToggle( "DRAW", &rendererRef->mDraw );
	ui->addToggle( "ADDITIVE BLENDING", &rendererRef->mAdditiveBlending );
	ui->addToggle( "DEPTH WRITE", &rendererRef->mDepthWrite );
	ui->addToggle( "DEPTH READ", &rendererRef->mDepthTest );
	ui->addSpacer();
	return ui;
}

UIPanelRef Transform::setupSystemUI( UIPanelRef ui, SystemRef systemRef )
{
#if USE_WINDOW_CANVAS
#else
	ui->addSpacer();
#endif
	ui->addButton( "RESET", false )->setCallback( [this, systemRef]( bool value ) {
		if( value ) {
			systemRef->reset();
		}
	} );

	ui->addToggle( "UPDATE", systemRef->getUpdate() )
		->setCallback(
			[this, systemRef]( bool value ) { systemRef->setUpdate( value ); } );
	return ui;
}

UIPanelRef Transform::setupPhysicsUI( UIPanelRef ui )
{
	setupSystemUI( ui, mParticleSystemRef );
	auto numparticles = ui->addDialeri( "PARTICLES", mParticleSystemRef->getTotal(), 0, 1000000 );
	numparticles->setTrigger( Trigger::END );
	numparticles->setCallback( [this]( int value ) {
		//		if( mParticleSystemRef->getTotal() != value ) {
		mParticleSystemRef->setTotal( value );
		mPlexusSystemRef->setTotal( value );
		mTrailSystemRef->setUpdateBuffers( true );
		//		}
	} );
	ui->addSpacer();
	return ui;
}

UIPanelRef Transform::setupTrailUI( UIPanelRef ui )
{
	setupSystemUI( ui, mTrailSystemRef );
	auto trailLength = ui->addDialeri( "TOTAL", mTrailSystemRef->getTotal(), 0, 1000 );
	trailLength->setTrigger( Trigger::END );
	trailLength->setCallback( [this]( int value ) {
		mTrailSystemRef->setTotal( value );
		if( mRibbonRendererRef && mRibbonRendererRef->isInitialized() ) {
			mRibbonRendererRef->_setupBatch();
		}
	} );
	ui->addSpacer();
	return ui;
}

UIPanelRef Transform::setupPlexusUI( UIPanelRef ui )
{
	setupSystemUI( ui, mPlexusSystemRef );
	ui->addSpacer();
	return ui;
}

void Transform::setupBoxBatch()
{
	mOutputWindowRef->getRenderer()->makeCurrentContext();
	mBoxBatchRef = gl::Batch::create( geom::WireCube().size( mBoxSize * 2.0f ),
		gl::getStockShader( gl::ShaderDef().color() ) );
}

void Transform::drawBoxBatch()
{
	if( mDrawBox ) {
		gl::ScopedBlendAdditive scpAdd;
		gl::ScopedColor scpClr( mBoxColor );
		gl::ScopedDepthTest scpDrd( true );
		gl::ScopedDepthWrite scpDwt( true );
		mBoxBatchRef->draw();
	}
}

void Transform::setupGrid()
{
	mOutputWindowRef->getRenderer()->makeCurrentContext();
	if( mBoxSize.x > 0 && mBoxSize.y > 0 && mBoxSize.z > 0 ) {
		auto geo = geom::WirePlane()
					   .normal( vec3( 0.0f, 0.0f, 1.0f ) )
					   .size( 2.0f * vec2( mBoxSize.x, mBoxSize.y ) )
					   .subdivisions( ivec2( mGridSubdivisons ) );
		auto glsl = gl::getStockShader( gl::ShaderDef().color() );
		mGridLinesBatchRef = gl::Batch::create( geo, glsl );

		float incX = mBoxSize.x * 2.0f / float( mGridSubdivisons );
		float incY = mBoxSize.y * 2.0f / float( mGridSubdivisons );
		gl::VertBatch verts;
		for( float y = -mBoxSize.y; y <= mBoxSize.y; y += incY ) {
			for( float x = -mBoxSize.x; x <= mBoxSize.x; x += incX ) {
				verts.vertex( vec3( x, y, 0 ) );
			}
		}
		mGridPointsBatchRef = gl::Batch::create( verts, glsl );
	}
}

void Transform::drawGridLines()
{
	if( mDrawGridLines ) {
		gl::ScopedMatrices scpMtx;
		gl::rotate( 0.5f * static_cast<float>( M_PI ), vec3( 1.0f, 0.0f, 0.0f ) );
		gl::translate( vec3( 0.0f, 0.0f, mBoxSize.y ) );
		gl::ScopedColor scpClr( mGridLinesColor );
		gl::ScopedBlendAdditive scpAdd;
		gl::ScopedDepthTest scpDth( true );
		gl::ScopedDepthWrite scpDrt( true );
		mGridLinesBatchRef->draw();
	}
}

void Transform::drawGridPoints()
{
	if( mDrawGridPoints ) {
		gl::ScopedMatrices scpMtx;
		gl::rotate( 0.5f * static_cast<float>( M_PI ), vec3( 1.0f, 0.0f, 0.0f ) );
		gl::translate( vec3( 0.0f, 0.0f, mBoxSize.y ) );
		gl::pointSize( static_cast<float>( mGridPointSize ) );
		gl::ScopedColor scpClr( mGridPointsColor );
		gl::ScopedBlendAdditive scpAdd;
		gl::ScopedDepthTest scpDth( true );
		gl::ScopedDepthWrite scpDrt( true );
		mGridPointsBatchRef->draw();
	}
}

void Transform::updateSystems()
{
	for( auto &it : mSystems ) {
		setDefaultUniforms( it->getGlslProgRef() );
		it->update();
	}
}

void Transform::updateRenderers()
{
	for( auto &it : mRenderers ) {
		it->update();
	}
}

void Transform::drawRenderers()
{
	for( auto &it : mRenderers ) {
		setDefaultUniforms( it->getGlslProgRef() );
		it->draw();
	}
}

void Transform::setupPoints()
{
	mPointRendererRef = PointRenderer::create(
		mOutputWindowRef, getShadersPath( "Points/points.vert" ), getShadersPath( "Points/points.frag" ), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(POINTS_UI);
        if (ui != nullptr) {
          if (mPointRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(POINTS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mPointRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( POINTS_UI ) + " ERROR: " + string( exc.what() ) ); } );

	mPointRendererRef->setup();
	mRenderers.push_back( mPointRendererRef );
}

void Transform::setupSprites()
{
	ci::geom::Plane *geom = new ci::geom::Plane();
	geom->size( vec2( 0 ) );
	PrimitiveRenderer::Format fmt;
	fmt.source( static_cast<ci::geom::SourceRef>( geom ) );

	mSpriteRendererRef = PrimitiveRenderer::create(
		mOutputWindowRef,
		getShadersPath( "Sprites/sprites.vert" ),
		getShadersPath( "Sprites/sprites.frag" ),
		mParticleSystemRef,
		fmt,
		[this]() {
        auto ui = mUIRef->getUI(SPRITES_UI);
        if (ui != nullptr) {
            if (mSpriteRendererRef->isInitialized()) {
                mUIRef->saveUI(ui, getSettingsPath());
            }
        } },
		[this]( GlslParamsRef glslParams ) {
            auto ui = mUIRef->getUI(SPRITES_UI);
            if (ui != nullptr) {
                ui->clear();
                setupRendererUI(ui, mSpriteRendererRef);
                mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
                mUIRef->loadUI(ui, getSettingsPath());
            } },
		[this]( ci::Exception exc ) { CI_LOG_E( string( SPRITES_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mSpriteRendererRef->setup();
	mRenderers.push_back( mSpriteRendererRef );
}

void Transform::setupLines()
{
	mPlexusRendererRef = PlexusRenderer::create(
		mOutputWindowRef, getShadersPath( "Lines/lines.vert" ), getShadersPath( "Lines/lines.frag" ), mParticleSystemRef, mPlexusSystemRef, [this]() {
        auto ui = mUIRef->getUI(LINES_UI);
        if (ui != nullptr) {
          if (mPlexusRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(LINES_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mPlexusRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( LINES_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mPlexusRendererRef->setup();
	mRenderers.push_back( mPlexusRendererRef );
}

void Transform::setupTrailPoint()
{
	mTrailPointRendererRef = TrailPointRenderer::create(
		mOutputWindowRef, getShadersPath( "trail/points.vert" ), getShadersPath( "trail/points.frag" ), mParticleSystemRef, mTrailSystemRef, [this]() {
        auto ui = mUIRef->getUI(TRAIL_POINTS_UI);
        if (ui != nullptr) {
          if (mTrailPointRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(TRAIL_POINTS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mTrailPointRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( TRAIL_POINTS_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mTrailPointRendererRef->setup();
	mRenderers.push_back( mTrailPointRendererRef );
}

void Transform::setupRibbon()
{
	mRibbonRendererRef = RibbonRenderer::create(
		mOutputWindowRef, getShadersPath( "Ribbon/ribbon.vert" ), getShadersPath( "Ribbon/ribbon.frag" ), getShadersPath( "Ribbon/ribbon.glsl" ), mParticleSystemRef, mTrailSystemRef, [this]() {
        auto ui = mUIRef->getUI(RIBBON_UI);
        if (ui != nullptr) {
          if (mRibbonRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(RIBBON_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mRibbonRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( RIBBON_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mRibbonRendererRef->setup();
	mRenderers.push_back( mRibbonRendererRef );
}

void Transform::setupPrimitive()
{
	ci::geom::Plane *geom = new ci::geom::Plane();
	geom->size( vec2( 0 ) );
	PrimitiveRenderer::Format fmt;
	fmt.source( static_cast<ci::geom::SourceRef>( geom ) );

	mPrimitiveRendererRef = PrimitiveRenderer::create(
		mOutputWindowRef, getShadersPath( "Primitive/primitive.vert" ), getShadersPath( "Primitive/primitive.frag" ), mParticleSystemRef, PrimitiveRenderer::Format(), [this]() {
        auto ui = mUIRef->getUI(PRIMITIVE_UI);
        if (ui != nullptr) {
          if (mPrimitiveRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(PRIMITIVE_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mPrimitiveRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( PRIMITIVE_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mPrimitiveRendererRef->setup();
	mRenderers.push_back( mPrimitiveRendererRef );
}

void Transform::setupParticles()
{
	vector<string> varyings( 3 );
	varyings[PARTICLES_POS_INDEX] = "tf_position_mass";
	varyings[PARTICLES_VEL_INDEX] = "tf_velocity_mass";
	varyings[PARTICLES_INFO_INDEX] = "tf_info";

	gl::GlslProg::Format format;
	format.feedbackFormat( GL_SEPARATE_ATTRIBS )
		.feedbackVaryings( varyings )
		.attribLocation( "position_mass", PARTICLES_POS_INDEX )
		.attribLocation( "velocity_mass", PARTICLES_VEL_INDEX )
		.attribLocation( "info", PARTICLES_INFO_INDEX );

	mParticleSystemRef = ParticleSystem::create(
		mOutputWindowRef, getShadersPath( "Physics/physics.vert" ), format, [this]() {
        auto ui = mUIRef->getUI(PHYSICS_UI);
        if (ui != nullptr) {
          if (mParticleSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(PHYSICS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPhysicsUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( PHYSICS_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mSystems.push_back( mParticleSystemRef );
}

void Transform::setupPlexus()
{
	vector<string> varyings( 1 );
	varyings[PLEXUS_DISTANCE_INDEX] = "tf_plexus";

	gl::GlslProg::Format format;
	format.feedbackFormat( GL_SEPARATE_ATTRIBS )
		.feedbackVaryings( varyings )
		.attribLocation( "plexus", PLEXUS_DISTANCE_INDEX );

	mPlexusSystemRef = PlexusSystem::create(
		mOutputWindowRef, getShadersPath( "Plexus/plexus.vert" ), format, mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(PLEXUS_UI);
        if (ui != nullptr) {
          if (mPlexusSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(PLEXUS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPlexusUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( PLEXUS_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mSystems.push_back( mPlexusSystemRef );
}

void Transform::setupTrail()
{
	vector<string> varyings( 1 );
	varyings[TRAILS_POS_INDEX] = "tf_position_mass";

	gl::GlslProg::Format format;
	format.feedbackFormat( GL_SEPARATE_ATTRIBS )
		.feedbackVaryings( varyings )
		.attribLocation( "position_mass", TRAILS_POS_INDEX )
		.attribLocation( "info", TRAILS_INFO_INDEX );

	mTrailSystemRef = TrailSystem::create(
		mOutputWindowRef, getShadersPath( "Trail/trail.vert" ), format, mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(TRAIL_UI);
        if (ui != nullptr) {
          if (mTrailSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(TRAIL_UI);
        if (ui != nullptr) {
          ui->clear();
          setupTrailUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( TRAIL_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mSystems.push_back( mTrailSystemRef );
}

void Transform::setDefaultUniforms( gl::GlslProgRef glslProgRef )
{
	if( glslProgRef ) {
		vec2 size = mOutputWindowRef->getSize();
		chrono::system_clock::time_point now = chrono::system_clock::now();
		time_t tt = chrono::system_clock::to_time_t( now );
		tm local_tm = *localtime( &tt );

		float hours = local_tm.tm_hour + 1.0f;
		float minutes = hours * 60 + ( local_tm.tm_min + 1 );
		float seconds = minutes * 60 + ( local_tm.tm_sec );

		float renderScale = 1.0f;
		if( mSequenceSaverRef->isRecording() || mImageSaverRef->isRecording() ) {
			renderScale = static_cast<float>( mImageSaverRef->getSizeMultiplier() );
		}
		glslProgRef->uniform( "iBackgroundColor", mBgColor );
		glslProgRef->uniform( "iResolution", vec3( toPixels( size.x ), toPixels( size.y ), 0.0 ) );
		glslProgRef->uniform( "iAspect", mOutputWindowRef->getAspectRatio() );
		glslProgRef->uniform( "iGlobalTime", mSeconds );
		glslProgRef->uniform( "iNumParticles",
			float( mParticleSystemRef->getTotal() ) );
		glslProgRef->uniform( "iAnimationTime", mCurrentTime );
		glslProgRef->uniform( "iMouse", vec4( toPixels( mMouse.x ), toPixels( size.y - mMouse.y ), toPixels( mMouseClick.x ), toPixels( size.y - mMouseClick.y ) ) );
		glslProgRef->uniform( "iDate",
			vec4( local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday, seconds ) );
		glslProgRef->uniform( "iRenderScale", renderScale );
		glslProgRef->uniform( "iCamEyePos",
			mCameraRef->getCameraPersp().getEyePoint() );
		glslProgRef->uniform( "iCamViewDir",
			mCameraRef->getCameraPersp().getViewDirection() );

		glslProgRef->uniform( "iSpectrum", 2 );
		glslProgRef->uniform( "iAmplitude", 3 );
		glslProgRef->uniform( "iPalettes", 4 );
		if( mSpectrumTextureRef ) {
			mSpectrumTextureRef->bind( 2 );
		}
		if( mAmplitudeTextureRef ) {
			mAmplitudeTextureRef->bind( 3 );
		}
		if( mPaletteTextureRef ) {
			mPaletteTextureRef->bind( 4 );
		}
	}
}

void Transform::loadPalettes()
{
	ci::Surface32fRef surfaceRef = ci::Surface32f::create( 5, 992, false );

	auto path = getPath( "Data/colors.json" );
	if( fs::exists( path ) ) {
		try {
			JsonTree tree( loadFile( path ) );
			int total = tree.getNumChildren();
			cout << total << endl;
			for( int i = 0; i < total; i++ ) {
				auto palette = tree.getChild( i );
				int colors = palette.getNumChildren();
				for( int j = 0; j < colors; j++ ) {
					auto color = palette.getChild( j );
					string hexString = color.getValue();
					Color clr = Color::hex( strtoul( hexString.c_str(), NULL, 16 ) );
					surfaceRef->setPixel( ivec2( j, i ), clr );
				}
			}
		}
		catch( ci::Exception exc ) {
			std::cout << "ERROR LOADING COLORS: " << exc.what() << std::endl;
		}
	}
	writeImage( getPath( "Data/colors.png" ), *surfaceRef.get(), ImageTarget::Options().quality( 1.0 ) );
}

void Transform::setupPalettes()
{
	ci::gl::Texture2d::Format fmt;
	fmt.magFilter( GL_NEAREST );
	fmt.minFilter( GL_NEAREST );
	mPaletteTextureRef = ci::gl::Texture2d::create( loadImage( getPath( "Data/colors.png" ) ), fmt );
}

void Transform::setupImageSaver()
{
	mImageSaverRef = ImageSaver::create(
		mOutputWindowRef, [this] { _drawOutput(); }, [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
        gl::clear(mBgColor);
        drawBg(ul, ur, lr, ll); }, [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
        gl::clear(mBgColor);
        drawPost(ul, ur, lr, ll); } );
}

void Transform::setupMovieSaver()
{
	mMovieSaverRef = MovieSaver::create( mOutputWindowRef );
}

void Transform::setupSequenceSaver()
{
	mSequenceSaverRef = SequenceSaver::create(
		mOutputWindowRef, [this] { _drawOutput(); }, [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
        gl::clear(mBgColor);
        drawBg(ul, ur, lr, ll); }, [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
        gl::clear(mBgColor);
        drawPost(ul, ur, lr, ll); } );
}

void Transform::setupAudio()
{
	setupAudioInput();
	setupAudioSpectrumTexture();
	setupAudioAmplitudeTexture();
}

void Transform::setupAudioSpectrumTexture()
{
	int length = 1024;
	for( int i = 0; i < length; i++ ) {
		mSpectrumBuffer.push_back( randFloat() );
	}

	auto fmt = gl::Texture2d::Format();
	fmt.dataType( GL_FLOAT );
	fmt.internalFormat( GL_R32F );
	mSpectrumTextureRef = gl::Texture2d::create( int( mSpectrumBuffer.size() ), 1.0, fmt );
}

void Transform::setupAudioAmplitudeTexture()
{
	int length = 1024;
	for( int i = 0; i < length; i++ ) {
		mAmplitudeBuffer.push_back( randFloat() );
	}

	auto fmt = gl::Texture2d::Format();
	fmt.dataType( GL_FLOAT );
	fmt.internalFormat( GL_R32F );
	mAmplitudeTextureRef = gl::Texture2d::create( int( mAmplitudeBuffer.size() ), 1.0, fmt );
}

void Transform::setupAudioInput()
{
	auto ctx = audio::Context::master();
	mInputDeviceNode = ctx->createInputDeviceNode();

	auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 2048 ).windowSize( 1024 );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( monitorFormat ) );

	mInputDeviceNode >> mMonitorSpectralNode;
	mInputDeviceNode->enable();
	ctx->enable();
}

void Transform::updateAudio()
{
	mSpectrumBuffer = mMonitorSpectralNode->getMagSpectrum();
	int length = int( mSpectrumBuffer.size() );
	for( int i = 0; i < length; i++ ) {
		mSpectrumBuffer[i] = audio::linearToDecibel( mSpectrumBuffer[i] ) / 100.0;
	}
	mSpectrumTextureRef->update( (void *)mSpectrumBuffer.data(), GL_RED, GL_FLOAT, 0, int( mSpectrumBuffer.size() ), 1.0 );

	auto buffer = mMonitorSpectralNode->getBuffer();
	int channels = buffer.getNumChannels();
	int ampLength = buffer.getSize() / channels;
	float *amplitudeData = buffer.getChannel( 0 );
	for( int i = 0; i < ampLength; i++ ) {
		mAmplitudeBuffer[i] = amplitudeData[i] * 2.0;
	}
	mAmplitudeTextureRef->update( (void *)mAmplitudeBuffer.data(), GL_RED, GL_FLOAT, 0, int( buffer.getNumFrames() ), 1.0 );
}

void Transform::setupPost()
{
	mRectRendererRef = RectRenderer::create(
		mOutputWindowRef, getShadersPath( "Post/post.vert" ), getShadersPath( "Post/post.frag" ), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(POST_UI);
        if (ui != nullptr) {
          if (mRectRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(POST_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPostUI(ui);
          setupRendererUI(ui, mRectRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( POST_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mRectRendererRef->setup();
}

void Transform::updatePost()
{
	mRectRendererRef->update();
}

void Transform::drawPost()
{
	gl::setMatricesWindow( mOutputWindowRef->getSize() );
	auto glslProgRef = mRectRendererRef->getGlslProgRef();
	if( glslProgRef ) {
		setDefaultUniforms( glslProgRef );
		mOutputFboRef->getColorTexture()->bind( 0 );
		glslProgRef->uniform( "iScreenTexture", 0 );
		mRectRendererRef->draw();
	}
}

void Transform::drawPost( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll )
{
	vec2 size = mOutputWindowRef->getSize();
	gl::setMatricesWindow( size );
	auto glslProgRef = mRectRendererRef->getGlslProgRef();
	if( glslProgRef ) {
		setDefaultUniforms( glslProgRef );
		glslProgRef->uniform( "iScreenTexture", 0 );
		auto batch = gl::Batch::create(
			geom::Rect( Rectf( 0.0f, 0.0f, size.x, size.y ) ).texCoords( ul, ur, lr, ll ),
			glslProgRef );
		batch->draw();
	}
}

void Transform::setupBg()
{
	mBgRendererRef = RectRenderer::create(
		mOutputWindowRef, getShadersPath( "Background/background.vert" ), getShadersPath( "Background/background.frag" ), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(BACKGROUND_UI);
        if (ui != nullptr) {
          if (mBgRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getSettingsPath());
          }
        } }, [this]( GlslParamsRef glslParams ) {
        auto ui = mUIRef->getUI(BACKGROUND_UI);
        if (ui != nullptr) {
          ui->clear();
          setupBgUI(ui);
          setupRendererUI(ui, mBgRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getSettingsPath());
        } }, [this]( ci::Exception exc ) { CI_LOG_E( string( BACKGROUND_UI ) + " ERROR: " + string( exc.what() ) ); } );
	mBgRendererRef->setup();
}

void Transform::updateBg()
{
	mBgRendererRef->update();
}

void Transform::drawBg()
{
	gl::setMatricesWindow( mOutputWindowRef->getSize() );
	gl::clear( mBgColor );
	auto glslProgRef = mBgRendererRef->getGlslProgRef();
	if( glslProgRef && mBgRendererRef->mDraw ) {
		setDefaultUniforms( glslProgRef );
		mOutputFboRef->getColorTexture()->bind( 0 );
		glslProgRef->uniform( "iScreenTexture", 0 );
		mBgRendererRef->draw();
	}
}

void Transform::drawBg( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll )
{
	vec2 size = mOutputWindowRef->getSize();
	gl::setMatricesWindow( size );
	auto glslProgRef = mBgRendererRef->getGlslProgRef();
	if( glslProgRef ) {
		setDefaultUniforms( glslProgRef );
		mOutputFboRef->getColorTexture()->bind( 0 );
		glslProgRef->uniform( "iScreenTexture", 0 );
		auto batch = gl::Batch::create(
			geom::Rect( Rectf( 0.0f, 0.0f, size.x, size.y ) ).texCoords( ul, ur, lr, ll ),
			glslProgRef );
		batch->draw();
	}
}

CINDER_APP( Transform, RendererGl( RendererGl::Options().msaa( 16 ) ), Transform::prepareSettings );