// STD
#include <algorithm>
#include <ctype.h>
#include <string>

// CINDER
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

#define CI_MIN_LOG_LEVEL 4
#include "cinder/Log.h"

//EXT
#include "cinder/audio/audio.h"
#include "cinder/osc/Osc.h"

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

#define USE_UDP 1

#if USE_UDP
using Receiver = osc::ReceiverUdp;
using protocol = asio::ip::udp;
typedef std::shared_ptr<class osc::ReceiverUdp> ReceiverRef;
#else
using Receiver = osc::ReceiverTcp;
using protocol = asio::ip::tcp;
typedef std::shared_ptr<class osc::ReceiverTcp> ReceiverRef;
#endif

enum PRIMITIVE {
    RECT,
    ROUNDEDRECT,
    CUBE,
    ICOSAHEDRON,
    ICOSPHERE,
    TEAPOT,
    CIRCLE,
    RING,
    SPHERE,
    CAPSULE,
    TORUS,
    TORUSKNOT,
    HELIX,
    CYLINDER,
    CONE,
    PLANE
};
/*
TODO:
+ Implement better UI system
+ Implemnt buffers for color & orientation
 
DONE:
+ Extend Primitive Renderer
+ Implement OSC for Params Control
+ Setup OSC UI
+ Fix Loading Issue
+ Export Points via ply
+ Integrate Color Palettes
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
 
ABANDONED:
+ Export Curves (MEL)
+ Export Meshes Spheres (OpenVDB)
+ Camera Animtion UI (Render to a 1 pixel fbo? pos, orientation quat)
*/

class Transform : public App
{
  public:
    // PREPARE SETTINGS
    static void prepareSettings(Settings *settings);

    // SETUP
    void setup() override;

    // CLEANUP
    void cleanup() override;

    // FS
    void createAssetDirectories();
    void createSessionDefaultDirectories();
    void createSessionWorkingDirectories();

    // EDITOR
    void openEditor();

    // SAVE & LOAD
    void save();
    void load();

    void saveSession();
    void loadSession();

    void saveAs(const fs::path &path);
    void saveSettings(const fs::path &path);
    void saveShaders(const fs::path &path);

    void loadAs(const fs::path &path);
    void loadSettings(const fs::path &path);
    void loadShaders(const fs::path &path);

    // OUTPUT
    gl::FboRef mOutputFboRef;
    void setupOutputFbo();
    void setupOutputWindow();
    void updateOutput();
    void drawOutput();
    void _drawOutput();
    void keyDownOutput(KeyEvent event);
    ci::app::WindowRef mOutputWindowRef;
    vec2 mMouse = vec2(0.0);
    vec2 mMousePrev = vec2(0.0);
    vec2 mMouseClick = vec2(0.0);
    ivec2 mOutputWindowOrigin = ivec2(0);
    ivec2 mOutputWindowSize = ivec2(1920, 1080);

    // UI
    AppUIRef mUIRef;
    void setupUIs();
    UIPanelRef setupAppUI(UIPanelRef ui);
    UIPanelRef setupAudioUI(UIPanelRef ui);
    UIPanelRef setupOscUI(UIPanelRef ui);
    UIPanelRef setupPostUI(UIPanelRef ui);
    UIPanelRef setupBgUI(UIPanelRef ui);
    UIPanelRef setupSystemUI(UIPanelRef ui, reza::ps::SystemRef systemRef);
    UIPanelRef setupPhysicsUI(UIPanelRef ui);
    UIPanelRef setupTrailUI(UIPanelRef ui);
    UIPanelRef setupPlexusUI(UIPanelRef ui);
    UIPanelRef setupExporterUI(UIPanelRef ui);
    UIPanelRef setupCameraUI(UIPanelRef ui);
    UIPanelRef setupRendererUI(UIPanelRef ui, reza::ps::RendererRef rendererRef);
    UIPanelRef setupPrimitiveUI(UIPanelRef ui, reza::ps::RendererRef rendererRef);
    UIPanelRef setupPrimitiveGeometryUI(UIPanelRef ui);

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
    vec3 mBoxSize = vec3(5.0f, 5.0f, 5.0f);
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
    void updateParticles();

    int mParticleTotal = 100;
    bool mUpdateParticleTotal = true;

    // PLEXUS SYSTEM
    PlexusSystemRef mPlexusSystemRef;
    void setupPlexus();

    // TRAIL SYSTEM
    TrailSystemRef mTrailSystemRef;
    void setupTrail();
    void updateTrail();
    int mTrailTotal = 100;
    bool mUpdateTrailTotal = true;

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
    void updatePrimitive();
    void updatePrimitiveGeometry();
    int mPrimitiveType = 0;
    ci::geom::Source *mPrimitiveGeometry = nullptr;
    bool mUpdatePrimitiveGeometry = true;

    //CONSIDER A REFACTOR

    //ROUNDED RECT
    float mRoundedRectCornerRadius = 0.2;
    int mRoundedRectCornerSubdivisions = 8;
    //CUBE
    int mCubeSubdivisions = 0;
    //ICOSPHERE
    int mIcoSphereSubdivisions = 2;
    //TEAPOT
    int mTeapotSubdivisions = 8;
    //CIRCLE
    int mCircleSubdivisions = 40;
    //RING
    int mRingSubdivisions = 40;
    float mRingRadius = 0.5f;
    float mRingWidth = 0.1f;
    //SPHERE
    int mSphereSubdivisions = 60;
    //CAPSULE
    float mCapsuleRadius = 0.5;
    float mCapsuleLength = 1.0;
    int mCapsuleAxisSubdivisions = 40;
    int mCapsuleHeightSubdivisions = 30;
    //TORUS
    float mTorusMajorRadius = 1.0;
    float mTorusMinorRadius = 0.8;
    int mTorusAxisSubdivisions = 60;
    int mTorusHeightSubdivisions = 60;
    int mTorusTwist = 0;
    float mTorusTwistOffset = 0;
    //TORUS KNOT
    int mTorusKnotP = 2;
    int mTorusKnotQ = 3;
    float mTorusKnotRadius = 0.25;
    int mTorusKnotAxisSubdivisions = 30;
    int mTorusKnotHeightSubdivisions = 120;
    //HELIX
    float mHelixCoils = 2.0;
    float mHelixHeight = 2.0;
    float mHelixMajorRadius = 1.0;
    float mHelixMinorRadius = 0.8;
    int mHelixAxisSubdivisions = 40;
    int mHelixHeightSubdivisions = 40;
    int mHelixTwist = 3;
    float mHelixTwistOffset = M_PI;
    //CYLINDER
    int mCylinderAxisSubdivisions = 40;
    int mCylinderHeightSubdivisions = 40;
    int mCylinderCapSubdivisions = 20;
    float mCylinderHeight = 1.0f;
    float mCylinderRadius = 0.5f;
    //CONE
    int mConeAxisSubdivisions = 40;
    int mConeHeightSubdivisions = 40;
    float mConeApexRadius = 0.0;
    float mConeBaseRadius = 0.5;
    float mConeHeight = 1.0;
    //PLANE
    int mPlaneSubdivisions = 0;

    // SHADER TOYIFIER
    void setDefaultUniforms(gl::GlslProgRef glslProgRef);

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

    // POINT EXPORTER
    void exportPoints(const ci::fs::path &path, const std::string &filename);

    // SAVING & LOADING
    ci::fs::path mDefaultRenderPath;
    fs::path mDefaultSaveLoadPath;
    fs::path mDefaultMoviePath;
    fs::path mDefaultExportPath;

    // AUDIO
    audio::InputDeviceNodeRef mInputDeviceNode;
    audio::MonitorSpectralNodeRef mMonitorSpectralNode;
    void setupAudio();
    void setupAudioSpectrumTexture();
    void setupAudioAmplitudeTexture();
    void setupAudioInput();
    void updateAudio();
    int mSampleSize;
    vector<float> mSpectrumBuffer;
    vector<float> mAmplitudeBuffer;

    ci::gl::Texture2dRef mSpectrumTextureRef;
    ci::gl::Texture2dRef mAmplitudeTextureRef;

    // POST
    RectRendererRef mRectRendererRef;
    void setupPost();
    void updatePost();
    void drawPost();
    void drawPost(const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll);

    // BACKGROUND
    RectRendererRef mBgRendererRef;
    void setupBg();
    void updateBg();
    void drawBg();
    void drawBg(const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll);

    // OSC
    ReceiverRef mReceiverRef;
    void setupOsc();
    int mOscPort = 10001;
};

void Transform::prepareSettings(App::Settings *settings)
{
    settings->setWindowSize(512, 512);
    settings->setFrameRate(60.0f);
    settings->setHighDensityDisplayEnabled();
}

void Transform::setup()
{
    //    gl::enableVerticalSync();

    cout << getAppSupportPath() << endl;
    CI_LOG_V("SETUP ASSETS DIRECTORIES");
    createAssetDirectories();
    CI_LOG_V("SETUP DEFAULT DIRECTORIES");
    createSessionDefaultDirectories();
    CI_LOG_V("SETUP WORKING DIRECTORIES");
    createSessionWorkingDirectories();

    CI_LOG_V("SETUP OUTPUT WINDOW");
    setupOutputWindow();
    CI_LOG_V("SETUP OUTPUT FBO");
    setupOutputFbo();

    CI_LOG_V("SETUP PALETTES");
    setupPalettes();

    CI_LOG_V("SETUP CAMERA");
    mCameraRef = EasyCamera::create(mOutputWindowRef);
    mCameraRef->enable();

    CI_LOG_V("SETUP SAVERS");
    setupImageSaver();
    setupSequenceSaver();
    setupMovieSaver();

    CI_LOG_V("SETUP DEBUG");
    setupBoxBatch();
    setupGrid();

    CI_LOG_V("SETUP PARTICLES");
    setupParticles();
    CI_LOG_V("SETUP PLEXUS");
    setupPlexus();
    CI_LOG_V("SETUP TRAIL");
    setupTrail();

    CI_LOG_V("SETUP POINTS");
    setupPoints();
    CI_LOG_V("SETUP SPRITES");
    setupSprites();
    CI_LOG_V("SETUP LINES");
    setupLines();
    CI_LOG_V("SETUP TRAIL POINTS");
    setupTrailPoint();
    CI_LOG_V("SETUP RIBBONS");
    setupRibbon();
    CI_LOG_V("SETUP PRIMITIVE");
    setupPrimitive();
    CI_LOG_V("SETUP POST");
    setupPost();
    CI_LOG_V("SETUP BG");
    setupBg();

    CI_LOG_V("SETUP AUDIO");
    setupAudio();

    CI_LOG_V("SETUP UI");
    setupUIs();

    CI_LOG_V("LOAD SETTINGS");
    loadSettings(getAppSupportWorkingSessionPath());
    CI_LOG_V("DONE LOADING");
}

void Transform::cleanup()
{
    if (mPrimitiveGeometry != nullptr) {
        delete mPrimitiveGeometry;
    }

    saveSettings(getAppSupportWorkingSessionPath());
}

void Transform::createAssetDirectories()
{
    //INSIDER THE APP
    auto localAssets = getResourcesAssetsPath();

    //OUTSIDE THE APP
    auto appAssets = getAppSupportAssetsPath();

    if (!fs::exists(appAssets)) {
        copyDirectory(localAssets, appAssets);
    }
}

void Transform::createSessionDefaultDirectories()
{
    //INSIDER THE APP
    auto localDefaultSession = getResourcesDefaultPath();
    auto localDefaultSettingsSession = getResourcesDefaultSettingsPath();
    auto localDefaultShadersSession = getResourcesDefaultShadersPath();

    //OUTSIDE THE APP
    auto appDefaultSession = getAppSupportDefaultSessionPath();
    auto appDefaultSettingsSession = getAppSupportDefaultSessionSettingsPath();
    auto appDefaultShadersSession = getAppSupportDefaultSessionShadersPath();

    if (!fs::exists(appDefaultSession)) {
        createDirectories(appDefaultSession);
        copyDirectory(localDefaultSettingsSession, appDefaultSettingsSession);
        copyDirectory(localDefaultShadersSession, appDefaultShadersSession);
    }
}

void Transform::createSessionWorkingDirectories()
{
    //INSIDER THE APP
    auto localWorkingSession = getResourcesWorkingPath();
    auto localWorkingSettingsSession = getResourcesWorkingSettingsPath();
    auto localWorkingShadersSession = getResourcesWorkingShadersPath();

    //OUTSIDE THE APP
    auto appWorkingSession = getAppSupportWorkingSessionPath();
    auto appWorkingSettingsSession = getAppSupportWorkingSessionSettingsPath();
    auto appWorkingShadersSession = getAppSupportWorkingSessionShadersPath();

    if (!fs::exists(appWorkingSession)) {
        createDirectories(appWorkingSession);
        copyDirectory(localWorkingSettingsSession, appWorkingSettingsSession);
        copyDirectory(localWorkingShadersSession, appWorkingShadersSession);
    }
}

void Transform::openEditor()
{
    auto shaderPath = getAppSupportWorkingSessionShadersPath();
    string cmd = "cd " + shaderPath.string() + " && open -a /Applications/Sublime\\ Text.app .";
    system(cmd.c_str());
}

void Transform::save()
{
    saveAs(getAppSupportWorkingSessionPath());
}

void Transform::saveAs(const fs::path &path)
{
    createDirectories(path);
    saveSettings(path);
    saveShaders(path);
}

void Transform::saveSettings(const fs::path &path)
{
    auto settingsPath = addPath(path, SETTINGS_PATH);
    saveWindow(addPath(settingsPath, WINDOW_PATH), mOutputWindowOrigin, mOutputWindowSize, true);
    saveCamera(addPath(settingsPath, CAMERA_PATH), mCameraRef->getCameraPersp());
    mUIRef->saveUIs(settingsPath);
}

void Transform::saveShaders(const fs::path &path)
{
    auto shadersPath = addPath(path, SHADERS_PATH);
    createDirectory(shadersPath);
    copyDirectory(getAppSupportWorkingSessionShadersPath(), shadersPath);
}

void Transform::load()
{
    loadAs(getAppSupportWorkingSessionPath());
}

void Transform::loadAs(const fs::path &path)
{
    loadSettings(path);
    loadShaders(path);
}

void Transform::loadSettings(const fs::path &path)
{
    auto settingsPath = addPath(path, SETTINGS_PATH);
    loadWindow(addPath(settingsPath, WINDOW_PATH), false, [this](ivec2 origin, ivec2 size, bool closed) {
        if (mOutputWindowRef) {
            mOutputWindowRef->setPos(origin);
            mOutputWindowRef->setSize(size);
        }
    });
    loadCamera(addPath(settingsPath, CAMERA_PATH), mCameraRef->getCameraPersp(), [this]() { mCameraRef->update(); });
    mUIRef->loadUIs(settingsPath);
}

void Transform::loadShaders(const fs::path &path)
{
    auto shadersPath = addPath(path, SHADERS_PATH);
    copyDirectory(shadersPath, getAppSupportWorkingSessionShadersPath());
}

void Transform::setupOutputWindow()
{
    mOutputWindowRef = getWindow();
    mOutputWindowRef->getSignalClose().connect([this] { quit(); });
    mOutputWindowRef->getSignalDraw().connect([this] {
        updateOutput();
        mImageSaverRef->update();
        mSequenceSaverRef->update();
        drawOutput();
        mMovieSaverRef->update();
    });
    mOutputWindowRef->getSignalResize().connect([this] {
        mOutputWindowSize = mOutputWindowRef->getSize();
        saveCamera(getAppSupportWorkingSessionSettingsPath(CAMERA_PATH), mCameraRef->getCameraPersp());
        mCameraRef->setup();
        loadCamera(getAppSupportWorkingSessionSettingsPath(CAMERA_PATH), mCameraRef->getCameraPersp(), [this]() { mCameraRef->update(); });
        setupOutputFbo();
        if (mRectRendererRef) {
            mRectRendererRef->setupBatch();
        }
        if (mBgRendererRef) {
            mBgRendererRef->setupBatch();
        }
    });
    mOutputWindowRef->getSignalMove().connect(
        [this] { mOutputWindowOrigin = mOutputWindowRef->getPos(); });
    mOutputWindowRef->getSignalKeyDown().connect(
        [this](KeyEvent event) { keyDownOutput(event); });
    mOutputWindowRef->getSignalMouseDown().connect([this](MouseEvent event) {
        if (mUIRef) {
            if (mUIRef->isHit(event.getPos())) {
                if (mCameraRef) {
                    mCameraRef->disable();
                }
            }
        }
        mMousePrev = mMouseClick = mMouse = vec2(event.getPos());
    });
    mOutputWindowRef->getSignalMouseDrag().connect([this](MouseEvent event) {
        mMousePrev = mMouse;
        mMouse = event.getPos();
    });
    mOutputWindowRef->getSignalMouseUp().connect([this](MouseEvent event) {
        mMouse = vec2(0.0);
        if (mCameraRef) {
            mCameraRef->enable();
        }
    });
}

void Transform::setupOutputFbo()
{
    gl::Fbo::Format fmt;
//    fmt.samples( gl::Fbo::getMaxSamples() );
    mOutputFboRef = gl::Fbo::create(toPixels(mOutputWindowRef->getWidth()),
                                    toPixels(mOutputWindowRef->getHeight()),
                                    fmt);
}

void Transform::updateOutput()
{
    mSeconds += 0.016f;
    mOutputWindowRef->setTitle(to_string((int)getAverageFps()) + " FPS");
    mOutputWindowRef->getRenderer()->makeCurrentContext();

    if (mSequenceSaverRef->isRecording()) {
        mCurrentTime = mSequenceSaverRef->getCurrentTime();
    } else if (mMovieSaverRef->isRecording()) {
        mCurrentTime = mMovieSaverRef->getCurrentTime();
    } else {
        mCurrentTime = mSequenceSaverRef->getCurrentTime();
    }

    updateAudio();
    updateParticles();
    updateTrail();
    updateSystems();
    updatePrimitive();
    updateRenderers();
    updateBg();
    updatePost();
}

void Transform::drawOutput()
{
    gl::clear(mBgColor);
    if (mOutputFboRef) {
        mOutputFboRef->bindFramebuffer();
        gl::ScopedViewport scpVp(ivec2(0), mOutputFboRef->getSize());
        drawBg();
        gl::setMatrices(mCameraRef->getCameraPersp());
    _drawOutput();
        mOutputFboRef->unbindFramebuffer();
        drawPost();
    }
    gl::setMatricesWindow(mOutputWindowRef->getSize());
}

void Transform::_drawOutput()
{
    ci::gl::lineWidth(4.0);
    drawBoxBatch();
    drawGridLines();
    drawGridPoints();
    drawRenderers();
}

void Transform::keyDownOutput(KeyEvent event)
{
    if (event.isMetaDown() || event.isControlDown()) {
        switch (event.getCode()) {
            case KeyEvent::KEY_p: {
                mUIRef->toggleUIs();
            } break;
            case KeyEvent::KEY_f: {
                mOutputWindowRef->setFullScreen(!mOutputWindowRef->isFullScreen());
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

            case KeyEvent::KEY_e: {
                openEditor();
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
        APP_UI, [this](UIPanelRef ui) -> UIPanelRef { return setupAppUI(ui); });
    mUIRef->setupUI(AUDIO_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupAudioUI(ui);
    });

    mUIRef->setupUI(OSC_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupOscUI(ui);
    });

    mUIRef->setupUI(BACKGROUND_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupBgUI(ui);
    });
    mUIRef->setupUI(
        POST_UI, [this](UIPanelRef ui) -> UIPanelRef { return setupPostUI(ui); });
    mUIRef->setupUI(CAMERA_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupCameraUI(ui);
    });
    mUIRef->setupUI(EXPORTER_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupExporterUI(ui);
    });

    // SYSTEMS
    mUIRef->setupUI(PHYSICS_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupPhysicsUI(ui);
    });
    mUIRef->setupUI(PLEXUS_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupPlexusUI(ui);
    });
    mUIRef->setupUI(TRAIL_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupTrailUI(ui);
    });

    // RENDERERS
    mUIRef->setupUI(SPRITES_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupRendererUI(ui, mSpriteRendererRef);
    });
    mUIRef->setupUI(POINTS_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupRendererUI(ui, mPointRendererRef);
    });
    mUIRef->setupUI(LINES_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupRendererUI(ui, mPlexusRendererRef);
    });
    mUIRef->setupUI(TRAIL_POINTS_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupRendererUI(ui, mTrailPointRendererRef);
    });
    mUIRef->setupUI(RIBBON_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupRendererUI(ui, mRibbonRendererRef);
    });
    mUIRef->setupUI(PRIMITIVE_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupPrimitiveUI(ui, mPrimitiveRendererRef);
    });
    mUIRef->setupUI(PRIMITIVE_GEOMETRY_UI, [this](UIPanelRef ui) -> UIPanelRef {
        return setupPrimitiveGeometryUI(ui);
    });
}

void Transform::saveSession()
{
    auto rtrim = [](string input, string key) {
        string result = "";
        size_t found = input.rfind(key);
        if (found != string::npos) {
            result = input.substr(0, found);
        }
        return result;
    };

    auto prePath = mDefaultSaveLoadPath;
    auto pth = getSaveFilePath(prePath);
    if (!pth.empty()) {
        if (createDirectory(pth)) {
            mDefaultSaveLoadPath = fs::path(rtrim(pth.string(), "/"));
            saveAs(pth);
        }
    }
}

void Transform::loadSession()
{
    auto pth = getFolderPath(mDefaultSaveLoadPath);
    if (!pth.empty()) {
        copyDirectory(addPath(pth, "Settings"), getAppSupportWorkingSessionSettingsPath());
        copyDirectory(addPath(pth, "Shaders"), getAppSupportWorkingSessionShadersPath());

        for (auto &it : mSystems) {
            it->setupGlsl();
            it->setInitialized(false);
        }

        for (auto &it : mRenderers) {
            it->setupGlsl();
            it->setInitialized(false);
        }

        mDefaultSaveLoadPath = pth.parent_path();
    }
}

UIPanelRef Transform::setupAppUI(UIPanelRef ui)
{
    ui->clear();
    ui->addSpacer();
    ui->addButton("SAVE SESSION", false)->setCallback([this](bool value) {
        if (value) {
            saveSession();
        }
    });

    ui->addButton("LOAD SESSION", false)->setCallback([this](bool value) {
        if (value) {
            loadSession();
            loadSettings(getAppSupportWorkingSessionPath());
        }
    });

    ui->addSpacer();

    ui->addButton("OPEN EDITOR", false)->setCallback([this](bool value) {
        if (value) {
            openEditor();
        }
    });

    ui->addSpacer();
    auto clrfmt = ColorPicker::Format().label(false);
    ui->addColorPicker("BACKGROUND COLOR", &mBgColor);
    ui->addColorPicker("BOX COLOR", &mBoxColor, clrfmt);
    ui->right();
    ui->addToggle("DRAW BOX", &mDrawBox);
    ui->down();
    ui->addSpacer();
    ui->addColorPicker("GRID LINES COLOR", &mGridLinesColor, clrfmt);
    ui->right();
    ui->addToggle("DRAW GRID LINES", &mDrawGridLines);
    ui->down();
    ui->addColorPicker("GRID POINTS COLOR", &mGridPointsColor, clrfmt);
    ui->right();
    ui->addToggle("DRAW GRID POINTS", &mDrawGridPoints);
    ui->down();
    ui->addSpacer();
    return ui;
}

UIPanelRef Transform::setupAudioUI(UIPanelRef ui)
{
    ui->clear();
    ui->addSpacer();

    map<string, string> shortToLongMap;
    vector<string> names;
    auto devices = audio::Device::getInputDevices();
    for (auto &it : devices) {
        string name = it->getName();
        if (name.length() > 35) {
            string shortName = name.substr(0, 35);
            names.push_back(shortName);
            shortToLongMap[shortName] = name;
        } else {
            names.push_back(name);
            shortToLongMap[name] = name;
        }
    }

    auto rfmt = Radio::Format().direction(Direction::SOUTH);

    auto input = ui->addRadio("INPUT", names, rfmt);
    input->setCallback([this, shortToLongMap](string name, bool value) {
        if (value) {
            auto it = shortToLongMap.find(name);
            if (it != shortToLongMap.end()) {
                auto longName = it->second;
                cout << longName << endl;
//                mMonitorSpectralNode->disconnectAll();
//                mInputDeviceNode->disconnectAll();
            }
        }
    });

    ui->addSpacer();
    ui->addTexture("AMPLITUDE", mAmplitudeTextureRef, TextureView::Format().height(32));
    ui->addTexture("SPECTRUM", mSpectrumTextureRef, TextureView::Format().height(32));
    ui->setTriggerOnLoad(false);
    return ui;
}

UIPanelRef Transform::setupOscUI(UIPanelRef ui)
{
    ui->clear();
    ui->addSpacer();
    auto dialer = ui->addDialeri("PORT", &mOscPort, 0, 65535);
    dialer->setTrigger(Trigger::END);
    dialer->setCallback([this](int value) {
        setupOsc();
    });
    return ui;
}

UIPanelRef Transform::setupPostUI(UIPanelRef ui)
{
    return ui;
}

UIPanelRef Transform::setupBgUI(UIPanelRef ui)
{
    return ui;
}

UIPanelRef Transform::setupCameraUI(UIPanelRef ui)
{
    ui->clear();
    ui->addSpacer();
    ui->addButton("RESET CAMERA", false)->setCallback([this](bool value) {
        if (value) {
            mCameraRef->setup();
        }
    });
    ui->addSliderf("FOV", &mCameraRef->getFov(), 0.0f, 180.0f)
        ->setCallback([this](float value) { mCameraRef->update(); });

    return ui;
}

UIPanelRef Transform::setupExporterUI(UIPanelRef ui)
{
    ui->clear();
    ui->addSpacer();
    ui->addButton("SAVE IMAGE AS", false)->setCallback([this](bool value) {
        if (value) {
            fs::path path = ci::app::getSaveFilePath(mDefaultRenderPath);
            if (!path.empty()) {
                mDefaultRenderPath = path.parent_path();
                string filename = path.filename().string();
                string ext = path.extension().string();
                string dir = path.parent_path().string();

                fs::path opath = fs::path(dir);
                auto it = filename.find(".");
                if (it != string::npos) {
                    filename = filename.substr(0, it);
                }
                vector<string> extensions = {"png", "jpg", "tif"};
                bool valid = false;
                for (auto it : extensions) {
                    if (it == ext) {
                        valid = true;
                        break;
                    }
                }
                if (!valid) {
                    ext = "png";
                }
                dispatchSync([this, opath, filename, ext]() {
                    //                    saveAs( addPath( opath, filename ) );
                    mImageSaverRef->save(mCameraRef->getCameraPersp(), opath, filename, ext);
                });
            }
        }
    });
    ui->down();
    ui->addDialeri("OUTPUT IMAGE SCALE", mImageSaverRef->getSizeMultiplier(), 1, 20)
        ->setCallback([this](int value) {
            mImageSaverRef->setSizeMultiplier(value);
            mSequenceSaverRef->setSizeMultiplier(value);
        });

    ui->addSpacer();
    ui->addButton("RENDER", false)->setCallback([this](bool value) {
        if (value && (mSaveMovie || mSaveSequence)) {
            fs::path path = getSaveFilePath(mDefaultMoviePath);
            if (!path.empty()) {
                mDefaultMoviePath = path.parent_path();

                string filename = path.filename().string();
                string dir = path.parent_path().string();
                fs::path opath = path.parent_path();

                auto it = filename.rfind(".");
                if (it != string::npos) {
                    filename = filename.substr(0, it);
                }

                auto savePath = addPath(opath, filename);
                cout << "savePath: " << savePath << endl;
                //                saveAs( savePath );

                if (mSaveMovie) {
                    mMovieSaverRef->save(opath, filename, "mov");
                }

                if (mSaveSequence) {
                    mSequenceSaverRef->save(mCameraRef->getCameraPersp(),
                                            addPath(opath, filename),
                                            filename,
                                            "png");
                }
            }
        }
    });
    ui->right();
    ui->addToggle("MOV", &mSaveMovie);
    ui->addToggle("PNG", &mSaveSequence);
    ui->addDialeri("FRAMES", &mTotalFrames, 0, 99999, Dialeri::Format().label(false))
        ->setCallback([this](int value) {
            mMovieSaverRef->setTotalFrames(value);
            mSequenceSaverRef->setTotalFrames(value);
        });
    ui->down();
    ui->setSliderHeight(8);
    ui->addSliderf("PROGRESS", &mCurrentTime, 0.0, 1.0, Sliderf::Format().label(false));
    ui->addSpacer();
    ui->addButton("EXPORT PLY", false)->setCallback([this](bool value) {
        if (value) {
            fs::path path = getSaveFilePath(mDefaultExportPath);
            if (!path.empty()) {
                mDefaultExportPath = path.parent_path();
                string filename = path.filename().string();
                string dir = path.parent_path().string() + fs::path::preferred_separator;
                fs::path opath = fs::path(dir);
                auto it = filename.rfind(".");
                if (it != string::npos) {
                    filename = filename.substr(0, it);
                }
                exportPoints(opath, filename);
            }
        }
    });
    return ui;
}

UIPanelRef Transform::setupRendererUI(UIPanelRef ui,
                                      reza::ps::RendererRef rendererRef)
{
#if USE_WINDOW_CANVAS
#else
    ui->addSpacer();
#endif
    ui->addToggle("DRAW", &rendererRef->mDraw);
    ui->addToggle("ADDITIVE BLENDING", &rendererRef->mAdditiveBlending);
    ui->addToggle("DEPTH WRITE", &rendererRef->mDepthWrite);
    ui->addToggle("DEPTH READ", &rendererRef->mDepthTest);
    ui->addSpacer();
    return ui;
}

UIPanelRef Transform::setupPrimitiveUI(UIPanelRef ui,
                                       reza::ps::RendererRef rendererRef)
{
    setupRendererUI(ui, rendererRef);
    map<int, string> types = {
        {0, "Rect"},
        {1, "RoundedRect"},
        {2, "Cube"},
        {3, "Icosahedron"},
        {4, "Icosphere"},
        {5, "Teapot"},
        {6, "Circle"},
        {7, "Ring"},
        {8, "Sphere"},
        {9, "Capsule"},
        {10, "Torus"},
        {11, "TorusKnot"},
        {12, "Helix"},
        {13, "Cylinder"},
        {14, "Cone"},
        {15, "Plane"}};
    auto slider = ui->addSlideri("TYPE", &mPrimitiveType, 0, (int)types.size() - 1);
    slider->setTrigger(Trigger::END);
    slider->setCallback([this, types](int value) {
        auto it = types.find(mPrimitiveType);
        if (it != types.end()) {
            auto ui = mUIRef->getUI(PRIMITIVE_GEOMETRY_UI);
            if (ui) {
                ui->clear();
                bool minified = ui->isMinified();
                setupPrimitiveGeometryUI(ui);
                ui->setMinified(minified);
            }
        }
    });
    return ui;
}

UIPanelRef Transform::setupPrimitiveGeometryUI(UIPanelRef ui)
{
    auto cbf = [this](float value) {
        mUpdatePrimitiveGeometry = true;
    };
    auto cbi = [this](int value) {
        mUpdatePrimitiveGeometry = true;
    };

    switch (mPrimitiveType) {
        case RECT: {
        } break;
        case ROUNDEDRECT: {
            ui->addSliderf("CORNER RADIUS", &mRoundedRectCornerRadius)->setCallback(cbf);
            ui->addSlideri("DIVISIONS", &mRoundedRectCornerSubdivisions, 0, 100)->setCallback(cbi);
        } break;
        case CUBE: {
            ui->addSlideri("DIVISIONS", &mCubeSubdivisions, 0, 400)->setCallback(cbi);
        } break;
        case ICOSAHEDRON: {
        } break;
        case ICOSPHERE: {
            ui->addSlideri("DIVISIONS", &mIcoSphereSubdivisions, 0, 6)->setCallback(cbi);
        } break;
        case TEAPOT: {
            ui->addSlideri("DIVISIONS", &mTeapotSubdivisions, 0, 12)->setCallback(cbi);
        } break;
        case CIRCLE: {
            ui->addSlideri("DIVISIONS", &mCircleSubdivisions, 0, 200)->setCallback(cbi);
        } break;
        case RING: {
            //RING
            ui->addSlideri("DIVISIONS", &mRingSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("RADIUS", &mRingRadius)->setCallback(cbf);
            ui->addSliderf("WIDTH", &mRingWidth)->setCallback(cbf);
        } break;
        case SPHERE: {
            ui->addSlideri("DIVISIONS", &mSphereSubdivisions, 0, 200)->setCallback(cbi);
        } break;
        case CAPSULE: {
            ui->addSlideri("AXIS DIVISIONS", &mCapsuleAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mCapsuleHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("RADIUS", &mCapsuleRadius)->setCallback(cbf);
            ui->addSliderf("LENGTH", &mCapsuleLength)->setCallback(cbf);
        } break;
        case TORUS: {
            ui->addSlideri("AXIS DIVISIONS", &mTorusAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mTorusHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("MINOR RADIUS", &mTorusMinorRadius)->setCallback(cbf);
            ui->addSliderf("MAJOR RADIUS", &mTorusMajorRadius)->setCallback(cbf);
            ui->addSlideri("TWIST", &mTorusTwist, 0, 200)->setCallback(cbi);
            ui->addSliderf("TWIST OFFSET", &mTorusTwistOffset, 0, M_PI)->setCallback(cbf);
        } break;
        case TORUSKNOT: {
            ui->addSlideri("AXIS DIVISIONS", &mTorusKnotAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mTorusKnotHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("RADIUS", &mTorusKnotRadius)->setCallback(cbf)->setCallback(cbf);
            ui->addSlideri("P", &mTorusKnotP, 0, 20)->setCallback(cbi);
            ui->addSlideri("Q", &mTorusKnotQ, 0, 20)->setCallback(cbi);
        } break;
        case HELIX: {
            ui->addSlideri("AXIS DIVISIONS", &mHelixAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mHelixHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("MINOR RADIUS", &mHelixMinorRadius)->setCallback(cbf);
            ui->addSliderf("MAJOR RADIUS", &mHelixMajorRadius)->setCallback(cbf);
            ui->addSliderf("COILS", &mHelixCoils, 0, 10)->setCallback(cbf);
            ui->addSliderf("HEIGHT", &mHelixHeight, 0.0, 5.0)->setCallback(cbf);
            ui->addSlideri("TWIST", &mHelixTwist, 0, 200)->setCallback(cbi);
            ui->addSliderf("TWIST OFFSET", &mHelixTwistOffset, 0, M_PI)->setCallback(cbf);
        } break;
        case CYLINDER: {
            ui->addSlideri("AXIS DIVISIONS", &mCylinderAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mCylinderHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("CAP DIVISIONS", &mCylinderCapSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("RADIUS", &mCylinderRadius)->setCallback(cbf);
            ui->addSliderf("HEIGHT", &mCylinderHeight)->setCallback(cbf);
        } break;
        case CONE: {
            ui->addSlideri("AXIS DIVISIONS", &mConeAxisSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSlideri("HEIGHT DIVISIONS", &mConeHeightSubdivisions, 0, 200)->setCallback(cbi);
            ui->addSliderf("APEX RADIUS", &mConeApexRadius)->setCallback(cbf);
            ui->addSliderf("BASE RADIUS", &mConeBaseRadius)->setCallback(cbf);
            ui->addSliderf("HEIGHT", &mConeHeight, 0, 2.0)->setCallback(cbf);
        } break;
        case PLANE: {
            ui->addSlideri("DIVISIONS", &mPlaneSubdivisions, 0, 200)->setCallback(cbi);
        } break;
        default:
            break;
    }

    mUpdatePrimitiveGeometry = true;
    ui->autoSizeToFitSubviews();
    return ui;
}

UIPanelRef Transform::setupSystemUI(UIPanelRef ui, SystemRef systemRef)
{
#if USE_WINDOW_CANVAS
#else
    ui->addSpacer();
#endif
    ui->addButton("RESET", false)->setCallback([this, systemRef](bool value) {
        if (value) {
            systemRef->reset();
        }
    });

    ui->addToggle("UPDATE", systemRef->getUpdate())
        ->setCallback(
            [this, systemRef](bool value) { systemRef->setUpdate(value); });
    return ui;
}

UIPanelRef Transform::setupPhysicsUI(UIPanelRef ui)
{
    setupSystemUI(ui, mParticleSystemRef);
    auto numparticles = ui->addDialeri("PARTICLES", mParticleSystemRef->getTotal(), 1, 1000000);
    numparticles->setTrigger(Trigger::END);
    numparticles->setCallback([this](int value) {
        //        if( mParticleTotal != value ) {
        mParticleTotal = value;
        mUpdateParticleTotal = true;
        //        }
    });
    ui->addSpacer();
    return ui;
}

UIPanelRef Transform::setupTrailUI(UIPanelRef ui)
{
    setupSystemUI(ui, mTrailSystemRef);
    auto trailLength = ui->addDialeri("TOTAL", mTrailSystemRef->getTotal(), 0, 1000);
    trailLength->setTrigger(Trigger::END);
    trailLength->setCallback([this](int value) {
        mUpdateTrailTotal = true;
        mTrailTotal = value;
    });
    ui->addSpacer();
    return ui;
}

UIPanelRef Transform::setupPlexusUI(UIPanelRef ui)
{
    setupSystemUI(ui, mPlexusSystemRef);
    ui->addSpacer();
    return ui;
}

void Transform::setupBoxBatch()
{
    mOutputWindowRef->getRenderer()->makeCurrentContext();
    mBoxBatchRef = gl::Batch::create(geom::WireCube().size(mBoxSize * 2.0f),
                                     gl::getStockShader(gl::ShaderDef().color()));
}

void Transform::drawBoxBatch()
{
    if (mDrawBox) {
        gl::ScopedBlendAdditive scpAdd;
        gl::ScopedColor scpClr(mBoxColor);
        gl::ScopedDepthTest scpDrd(true);
        gl::ScopedDepthWrite scpDwt(true);
        mBoxBatchRef->draw();
    }
}

void Transform::setupGrid()
{
    mOutputWindowRef->getRenderer()->makeCurrentContext();
    if (mBoxSize.x > 0 && mBoxSize.y > 0 && mBoxSize.z > 0) {
        auto geo = geom::WirePlane()
                       .normal(vec3(0.0f, 0.0f, 1.0f))
                       .size(2.0f * vec2(mBoxSize.x, mBoxSize.y))
                       .subdivisions(ivec2(mGridSubdivisons));
        auto glsl = gl::getStockShader(gl::ShaderDef().color());
        mGridLinesBatchRef = gl::Batch::create(geo, glsl);

        float incX = mBoxSize.x * 2.0f / float(mGridSubdivisons);
        float incY = mBoxSize.y * 2.0f / float(mGridSubdivisons);
        gl::VertBatch verts;
        for (float y = -mBoxSize.y; y <= mBoxSize.y; y += incY) {
            for (float x = -mBoxSize.x; x <= mBoxSize.x; x += incX) {
                verts.vertex(vec3(x, y, 0));
            }
        }
        mGridPointsBatchRef = gl::Batch::create(verts, glsl);
    }
}

void Transform::drawGridLines()
{
    if (mDrawGridLines) {
        gl::ScopedMatrices scpMtx;
        gl::rotate(0.5f * static_cast<float>(M_PI), vec3(1.0f, 0.0f, 0.0f));
        gl::translate(vec3(0.0f, 0.0f, mBoxSize.y));
        gl::ScopedColor scpClr(mGridLinesColor);
        gl::ScopedBlendAdditive scpAdd;
        gl::ScopedDepthTest scpDth(true);
        gl::ScopedDepthWrite scpDrt(true);
        mGridLinesBatchRef->draw();
    }
}

void Transform::drawGridPoints()
{
    if (mDrawGridPoints) {
        gl::ScopedMatrices scpMtx;
        gl::rotate(0.5f * static_cast<float>(M_PI), vec3(1.0f, 0.0f, 0.0f));
        gl::translate(vec3(0.0f, 0.0f, mBoxSize.y));
        gl::pointSize(static_cast<float>(mGridPointSize));
        gl::ScopedColor scpClr(mGridPointsColor);
        gl::ScopedBlendAdditive scpAdd;
        gl::ScopedDepthTest scpDth(true);
        gl::ScopedDepthWrite scpDrt(true);
        mGridPointsBatchRef->draw();
    }
}

void Transform::updateSystems()
{
    for (auto &it : mSystems) {
        setDefaultUniforms(it->getGlslProgRef());
        it->update();
    }
}

void Transform::updateRenderers()
{
    for (auto &it : mRenderers) {
        it->update();
    }
}

void Transform::drawRenderers()
{
    for (auto &it : mRenderers) {
        setDefaultUniforms(it->getGlslProgRef());
        it->draw();
    }
}

void Transform::setupPoints()
{
    mPointRendererRef = PointRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Points/points.vert"), getAppSupportWorkingSessionShadersPath("Points/points.frag"), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(POINTS_UI);
        if (ui != nullptr) {
          if (mPointRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(POINTS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mPointRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(POINTS_UI) + " ERROR: " + string(exc.what())); });

    mPointRendererRef->setup();
    mRenderers.push_back(mPointRendererRef);
}

void Transform::setupSprites()
{
    ci::geom::Plane *geom = new ci::geom::Plane();
    geom->size(vec2(0));
    PrimitiveRenderer::Format fmt;
    fmt.geometry(geom);

    mSpriteRendererRef = PrimitiveRenderer::create(
        mOutputWindowRef,
        getAppSupportWorkingSessionShadersPath("Sprites/sprites.vert"),
        getAppSupportWorkingSessionShadersPath("Sprites/sprites.frag"),
        mParticleSystemRef,
        fmt,
        [this]() {
        auto ui = mUIRef->getUI(SPRITES_UI);
        if (ui != nullptr) {
            if (mSpriteRendererRef->isInitialized()) {
                mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
            }
        } },
        [this](GlslParamsRef glslParams) {
            auto ui = mUIRef->getUI(SPRITES_UI);
            if (ui != nullptr) {
                ui->clear();
                setupRendererUI(ui, mSpriteRendererRef);
                mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
                mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
            } },
        [this](ci::Exception exc) { CI_LOG_E(string(SPRITES_UI) + " ERROR: " + string(exc.what())); });
    mSpriteRendererRef->setup();
    mRenderers.push_back(mSpriteRendererRef);
}

void Transform::setupLines()
{
    mPlexusRendererRef = PlexusRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Lines/lines.vert"), getAppSupportWorkingSessionShadersPath("Lines/lines.frag"), mParticleSystemRef, mPlexusSystemRef, [this]() {
        auto ui = mUIRef->getUI(LINES_UI);
        if (ui != nullptr) {
          if (mPlexusRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(LINES_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mPlexusRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(LINES_UI) + " ERROR: " + string(exc.what())); });
    mPlexusRendererRef->setup();
    mRenderers.push_back(mPlexusRendererRef);
}

void Transform::setupTrailPoint()
{
    mTrailPointRendererRef = TrailPointRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("trail/points.vert"), getAppSupportWorkingSessionShadersPath("trail/points.frag"), mParticleSystemRef, mTrailSystemRef, [this]() {
        auto ui = mUIRef->getUI(TRAIL_POINTS_UI);
        if (ui != nullptr) {
          if (mTrailPointRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(TRAIL_POINTS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mTrailPointRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(TRAIL_POINTS_UI) + " ERROR: " + string(exc.what())); });
    mTrailPointRendererRef->setup();
    mRenderers.push_back(mTrailPointRendererRef);
}

void Transform::setupRibbon()
{
    mRibbonRendererRef = RibbonRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Ribbon/ribbon.vert"), getAppSupportWorkingSessionShadersPath("Ribbon/ribbon.frag"), getAppSupportWorkingSessionShadersPath("Ribbon/ribbon.glsl"), mParticleSystemRef, mTrailSystemRef, [this]() {
        auto ui = mUIRef->getUI(RIBBON_UI);
        if (ui != nullptr) {
          if (mRibbonRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(RIBBON_UI);
        if (ui != nullptr) {
          ui->clear();
          setupRendererUI(ui, mRibbonRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(RIBBON_UI) + " ERROR: " + string(exc.what())); });
    mRibbonRendererRef->setup();
    mRenderers.push_back(mRibbonRendererRef);
}

void Transform::setupPrimitive()
{
    ci::geom::Plane *geom = new ci::geom::Plane();
    geom->size(vec2(0));
    PrimitiveRenderer::Format fmt;
    fmt.geometry(geom);

    mPrimitiveRendererRef = PrimitiveRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Primitive/primitive.vert"), getAppSupportWorkingSessionShadersPath("Primitive/primitive.frag"), mParticleSystemRef, PrimitiveRenderer::Format(), [this]() {
        auto ui = mUIRef->getUI(PRIMITIVE_UI);
        if (ui != nullptr) {
          if (mPrimitiveRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(PRIMITIVE_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPrimitiveUI(ui, mPrimitiveRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(PRIMITIVE_UI) + " ERROR: " + string(exc.what())); });
    mPrimitiveRendererRef->setup();
    mRenderers.push_back(mPrimitiveRendererRef);
}

void Transform::updatePrimitive()
{
    if (mUpdatePrimitiveGeometry && mPrimitiveRendererRef) {
        updatePrimitiveGeometry();
        if (mPrimitiveGeometry) {
            mPrimitiveRendererRef->setGeometry(mPrimitiveGeometry);
            mUpdatePrimitiveGeometry = false;
        }
    }
}

void Transform::updatePrimitiveGeometry()
{
    switch (mPrimitiveType) {
        case RECT: {
            ci::geom::Rect *geom = new ci::geom::Rect();
            mPrimitiveGeometry = geom;
        } break;
        case ROUNDEDRECT: {
            ci::geom::RoundedRect *geom = new ci::geom::RoundedRect();
            geom->cornerRadius(mRoundedRectCornerRadius);
            geom->cornerSubdivisions(mRoundedRectCornerSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case CUBE: {
            ci::geom::Cube *geom = new ci::geom::Cube();
            geom->subdivisions(mCubeSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case ICOSAHEDRON: {
            ci::geom::Icosahedron *geom = new ci::geom::Icosahedron();
            mPrimitiveGeometry = geom;
        } break;
        case ICOSPHERE: {
            ci::geom::Icosphere *geom = new ci::geom::Icosphere();
            geom->subdivisions(mIcoSphereSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case TEAPOT: {
            ci::geom::Teapot *geom = new ci::geom::Teapot();
            geom->subdivisions(mTeapotSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case CIRCLE: {
            ci::geom::Circle *geom = new ci::geom::Circle();
            geom->subdivisions(mCircleSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case RING: {
            ci::geom::Ring *geom = new ci::geom::Ring();
            geom->radius(mRingRadius);
            geom->width(mRingWidth);
            geom->subdivisions(mRingSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case SPHERE: {
            ci::geom::Sphere *geom = new ci::geom::Sphere();
            geom->subdivisions(mSphereSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case CAPSULE: {
            ci::geom::Capsule *geom = new ci::geom::Capsule();
            geom->length(mCapsuleLength);
            geom->radius(mCapsuleRadius);
            geom->subdivisionsAxis(mCapsuleAxisSubdivisions);
            geom->subdivisionsHeight(mCapsuleHeightSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case TORUS: {
            ci::geom::Torus *geom = new ci::geom::Torus();
            geom->radius(mTorusMajorRadius, mTorusMinorRadius);
            geom->subdivisionsAxis(mTorusAxisSubdivisions);
            geom->subdivisionsHeight(mTorusHeightSubdivisions);
            geom->twist(mTorusTwist, mTorusTwistOffset);
            mPrimitiveGeometry = geom;
        } break;
        case TORUSKNOT: {
            ci::geom::TorusKnot *geom = new ci::geom::TorusKnot();
            geom->parameters(mTorusKnotP, mTorusKnotQ);
            geom->radius(mTorusKnotRadius);
            geom->subdivisionsAxis(mTorusKnotAxisSubdivisions);
            geom->subdivisionsHeight(mTorusKnotHeightSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case HELIX: {
            ci::geom::Helix *geom = new ci::geom::Helix();
            geom->coils(mHelixCoils);
            geom->height(mHelixHeight);
            geom->radius(mHelixMajorRadius, mHelixMinorRadius);
            geom->subdivisionsAxis(mHelixAxisSubdivisions);
            geom->subdivisionsHeight(mHelixHeightSubdivisions);
            geom->twist(mHelixTwist, mHelixTwistOffset);
            mPrimitiveGeometry = geom;
        } break;
        case CYLINDER: {
            ci::geom::Cylinder *geom = new ci::geom::Cylinder();
            geom->height(mCylinderHeight);
            geom->radius(mCylinderRadius);
            geom->subdivisionsAxis(mCylinderAxisSubdivisions);
            geom->subdivisionsHeight(mCylinderHeightSubdivisions);
            geom->subdivisionsCap(mCylinderCapSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case CONE: {
            ci::geom::Cone *geom = new ci::geom::Cone();
            geom->height(mConeHeight);
            geom->radius(mConeBaseRadius, mConeApexRadius);
            geom->subdivisionsAxis(mConeAxisSubdivisions);
            geom->subdivisionsHeight(mConeHeightSubdivisions);
            mPrimitiveGeometry = geom;
        } break;
        case PLANE: {
            ci::geom::Plane *geom = new ci::geom::Plane();
            geom->subdivisions(ivec2(mPlaneSubdivisions));
            mPrimitiveGeometry = geom;
        } break;
        default:
            break;
    }
}

void Transform::setupParticles()
{
    vector<string> varyings(4);
    varyings[PARTICLES_POS_INDEX] = "tf_position_id";
    varyings[PARTICLES_VEL_INDEX] = "tf_velocity_mass";
    varyings[PARTICLES_CLR_INDEX] = "tf_color";
    varyings[PARTICLES_ORI_INDEX] = "tf_orientation";

    gl::GlslProg::Format format;
    format.feedbackFormat(GL_SEPARATE_ATTRIBS)
        .feedbackVaryings(varyings)
        .attribLocation("position_id", PARTICLES_POS_INDEX)
        .attribLocation("velocity_mass", PARTICLES_VEL_INDEX)
        .attribLocation("color", PARTICLES_CLR_INDEX)
        .attribLocation("orientation", PARTICLES_ORI_INDEX);

    mParticleSystemRef = ParticleSystem::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Physics/physics.vert"), format, [this]() {
        auto ui = mUIRef->getUI(PHYSICS_UI);
        if (ui != nullptr) {
          if (mParticleSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(PHYSICS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPhysicsUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(PHYSICS_UI) + " ERROR: " + string(exc.what())); });
    mSystems.push_back(mParticleSystemRef);
}

void Transform::updateParticles()
{
    if (mUpdateParticleTotal && mParticleSystemRef && mPlexusSystemRef && mTrailSystemRef) {
        mParticleSystemRef->setTotal(mParticleTotal);
        mPlexusSystemRef->setTotal(mParticleTotal);
        mTrailSystemRef->setUpdateBuffers(true);
        mUpdateParticleTotal = false;
    }
}

void Transform::setupPlexus()
{
    vector<string> varyings(1);
    varyings[PLEXUS_DISTANCE_INDEX] = "tf_plexus";

    gl::GlslProg::Format format;
    format.feedbackFormat(GL_SEPARATE_ATTRIBS)
        .feedbackVaryings(varyings)
        .attribLocation("plexus", PLEXUS_DISTANCE_INDEX);

    mPlexusSystemRef = PlexusSystem::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Plexus/plexus.vert"), format, mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(PLEXUS_UI);
        if (ui != nullptr) {
          if (mPlexusSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(PLEXUS_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPlexusUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(PLEXUS_UI) + " ERROR: " + string(exc.what())); });
    mSystems.push_back(mPlexusSystemRef);
}

void Transform::setupTrail()
{
    vector<string> varyings(1);
    varyings[TRAILS_POS_INDEX] = "tf_position_mass";

    gl::GlslProg::Format format;
    format.feedbackFormat(GL_SEPARATE_ATTRIBS)
        .feedbackVaryings(varyings)
        .attribLocation("position_mass", TRAILS_POS_INDEX)
        .attribLocation("info", TRAILS_INFO_INDEX);

    mTrailSystemRef = TrailSystem::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Trail/trail.vert"), format, mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(TRAIL_UI);
        if (ui != nullptr) {
          if (mTrailSystemRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(TRAIL_UI);
        if (ui != nullptr) {
          ui->clear();
          setupTrailUI(ui);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(TRAIL_UI) + " ERROR: " + string(exc.what())); });
    mSystems.push_back(mTrailSystemRef);
}

void Transform::updateTrail()
{
    if (mUpdateTrailTotal && mTrailSystemRef) {
        mTrailSystemRef->setTotal(mTrailTotal);
        if (mRibbonRendererRef && mRibbonRendererRef->isInitialized()) {
            mRibbonRendererRef->_setupBatch();
        }
        mUpdateTrailTotal = false;
    }
}

void Transform::setDefaultUniforms(gl::GlslProgRef glslProgRef)
{
    if (glslProgRef) {
        vec2 size = mOutputWindowRef->getSize();
        chrono::system_clock::time_point now = chrono::system_clock::now();
        time_t tt = chrono::system_clock::to_time_t(now);
        tm local_tm = *localtime(&tt);

        float hours = local_tm.tm_hour + 1.0f;
        float minutes = hours * 60 + (local_tm.tm_min + 1);
        float seconds = minutes * 60 + (local_tm.tm_sec);

        float renderScale = 1.0f;
        if (mSequenceSaverRef->isRecording() || mImageSaverRef->isRecording()) {
            renderScale = static_cast<float>(mImageSaverRef->getSizeMultiplier());
        }
        glslProgRef->uniform("iBackgroundColor", mBgColor);
        glslProgRef->uniform("iResolution", vec3(toPixels(size.x), toPixels(size.y), 0.0));
        glslProgRef->uniform("iAspect", mOutputWindowRef->getAspectRatio());
        glslProgRef->uniform("iGlobalTime", mSeconds);
        glslProgRef->uniform("iNumParticles",
                             float(mParticleSystemRef->getTotal()));
        glslProgRef->uniform("iAnimationTime", mCurrentTime);
        glslProgRef->uniform("iMouse", vec4(toPixels(mMouse.x), toPixels(size.y - mMouse.y), toPixels(mMouseClick.x), toPixels(size.y - mMouseClick.y)));
        glslProgRef->uniform("iDate",
                             vec4(local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday, seconds));
        glslProgRef->uniform("iRenderScale", renderScale);
        glslProgRef->uniform("iCamEyePos",
                             mCameraRef->getCameraPersp().getEyePoint());
        glslProgRef->uniform("iCamViewDir",
                             mCameraRef->getCameraPersp().getViewDirection());

        glslProgRef->uniform("iSpectrum", 2);
        glslProgRef->uniform("iAmplitude", 3);
        glslProgRef->uniform("iPalettes", 4);
        if (mSpectrumTextureRef) {
            mSpectrumTextureRef->bind(2);
        }
        if (mAmplitudeTextureRef) {
            mAmplitudeTextureRef->bind(3);
        }
        if (mPaletteTextureRef) {
            mPaletteTextureRef->bind(4);
        }
    }
}

void Transform::loadPalettes()
{
    ci::Surface32fRef surfaceRef = ci::Surface32f::create(5, 992, false);
    auto path = getAppSupportJsonAssetsPath("colors.json");
    if (fs::exists(path)) {
        try {
            JsonTree tree(loadFile(path));
            int total = tree.getNumChildren();
            for (int i = 0; i < total; i++) {
                auto palette = tree.getChild(i);
                int colors = palette.getNumChildren();
                for (int j = 0; j < colors; j++) {
                    auto color = palette.getChild(j);
                    string hexString = color.getValue();
                    Color clr = Color::hex(strtoul(hexString.c_str(), NULL, 16));
                    surfaceRef->setPixel(ivec2(j, i), clr);
                }
            }
        } catch (ci::Exception exc) {
            std::cout << "ERROR LOADING COLORS: " << exc.what() << std::endl;
        }
    }
    writeImage(getAppSupportImageAssetsPath("colors.png"), *surfaceRef.get(), ImageTarget::Options().quality(1.0));
}

void Transform::setupPalettes()
{
    ci::gl::Texture2d::Format fmt;
    fmt.magFilter(GL_NEAREST);
    fmt.minFilter(GL_NEAREST);
    mPaletteTextureRef = ci::gl::Texture2d::create(loadImage(getAppSupportImageAssetsPath("colors.png")), fmt);
}

void Transform::setupImageSaver()
{
    mImageSaverRef = ImageSaver::create(
        mOutputWindowRef, [this] { _drawOutput(); }, [this](const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll) {
        gl::clear(mBgColor);
        drawBg(ul, ur, lr, ll); }, [this](const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll) {
        gl::clear(mBgColor);
        drawPost(ul, ur, lr, ll); });
}

void Transform::setupMovieSaver()
{
    mMovieSaverRef = MovieSaver::create(mOutputWindowRef);
}

void Transform::setupSequenceSaver()
{
    mSequenceSaverRef = SequenceSaver::create(
        mOutputWindowRef, [this] { _drawOutput(); }, [this](const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll) {
        gl::clear(mBgColor);
        drawBg(ul, ur, lr, ll); }, [this](const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll) {
        gl::clear(mBgColor);
        drawPost(ul, ur, lr, ll); });
}

void Transform::exportPoints(const ci::fs::path &path, const std::string &filename)
{
    fs::path final = path;
    final += fs::path(filename);
    final += fs::path(".ply");
    ofstream stream(final.string(), ios_base::out);
    stream << "ply" << endl;
    stream << "format ascii 1.0" << endl;
    stream << "element vertex " << mParticleSystemRef->getTotal() << endl;
    stream << "property float32 x" << endl;
    stream << "property float32 y" << endl;
    stream << "property float32 z" << endl;
    stream << "property float32 w" << endl;
    stream << "end_header" << endl;

    ci::gl::BufferTextureRef buffer = mParticleSystemRef->getPositionBufferTextureRef(0);
    ci::gl::BufferObjRef &bufObj = buffer->getBufferObj();
    float *data = (float *)bufObj->map(GL_READ_ONLY);
    int total = mParticleSystemRef->getTotal() * 4;
    for (int i = 0; i < total; i += 4) {
        stream << data[i] << " " << data[i + 1] << " " << data[i + 2] << " " << data[i + 3] << endl;
    }
    bufObj->unmap();
    stream.close();
}

void Transform::setupAudio()
{
    setupAudioInput();
    setupAudioSpectrumTexture();
    setupAudioAmplitudeTexture();
}

void Transform::setupAudioSpectrumTexture()
{
    int length = mSampleSize;
    for (int i = 0; i < length; i++) {
        mSpectrumBuffer.push_back(0);
    }

    auto fmt = gl::Texture2d::Format();
    fmt.dataType(GL_FLOAT);
    fmt.internalFormat(GL_R32F);
    mSpectrumTextureRef = gl::Texture2d::create(int(mSpectrumBuffer.size()), 1.0, fmt);
}

void Transform::setupAudioAmplitudeTexture()
{
    int length = mSampleSize;
    for (int i = 0; i < length; i++) {
        mAmplitudeBuffer.push_back(0);
    }

    auto fmt = gl::Texture2d::Format();
    fmt.dataType(GL_FLOAT);
    fmt.internalFormat(GL_R32F);
    mAmplitudeTextureRef = gl::Texture2d::create(int(mAmplitudeBuffer.size()), 1.0, fmt);
}

void Transform::setupAudioInput()
{
    mSampleSize = 512;
    
    auto ctx = audio::Context::master();
    mInputDeviceNode = ctx->createInputDeviceNode();
    
    auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( mSampleSize );
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
    int channels = (int)buffer.getNumChannels();
    int ampLength = (int)buffer.getSize() / (int)channels;
    float *amplitudeData = buffer.getChannel( 0 );
    for( int i = 0; i < ampLength; i++ ) {
        mAmplitudeBuffer[i] = amplitudeData[i] * 2.0;
    }
    mAmplitudeTextureRef->update( (void *)mAmplitudeBuffer.data(), GL_RED, GL_FLOAT, 0, int( mAmplitudeBuffer.size() ), 1.0 );
}

void Transform::setupPost()
{
    mRectRendererRef = RectRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Post/post.vert"), getAppSupportWorkingSessionShadersPath("Post/post.frag"), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(POST_UI);
        if (ui != nullptr) {
          if (mRectRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(POST_UI);
        if (ui != nullptr) {
          ui->clear();
          setupPostUI(ui);
          setupRendererUI(ui, mRectRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(POST_UI) + " ERROR: " + string(exc.what())); });
    mRectRendererRef->setup();
}

void Transform::updatePost()
{
    mRectRendererRef->update();
}

void Transform::drawPost()
{
    gl::setMatricesWindow(mOutputWindowRef->getSize());
    auto glslProgRef = mRectRendererRef->getGlslProgRef();
    if (glslProgRef) {
        setDefaultUniforms(glslProgRef);
        mOutputFboRef->getColorTexture()->bind(0);
        glslProgRef->uniform("iScreenTexture", 0);
        mRectRendererRef->draw();
    }
}

void Transform::drawPost(const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll)
{
    vec2 size = mOutputWindowRef->getSize();
    gl::setMatricesWindow(size);
    auto glslProgRef = mRectRendererRef->getGlslProgRef();
    if (glslProgRef) {
        setDefaultUniforms(glslProgRef);
        glslProgRef->uniform("iScreenTexture", 0);
        auto batch = gl::Batch::create(
            geom::Rect(Rectf(0.0f, 0.0f, size.x, size.y)).texCoords(ul, ur, lr, ll),
            glslProgRef);
        batch->draw();
    }
}

void Transform::setupBg()
{
    mBgRendererRef = RectRenderer::create(
        mOutputWindowRef, getAppSupportWorkingSessionShadersPath("Background/background.vert"), getAppSupportWorkingSessionShadersPath("Background/background.frag"), mParticleSystemRef, [this]() {
        auto ui = mUIRef->getUI(BACKGROUND_UI);
        if (ui != nullptr) {
          if (mBgRendererRef->isInitialized()) {
            mUIRef->saveUI(ui, getAppSupportWorkingSessionSettingsPath());
          }
        } }, [this](GlslParamsRef glslParams) {
        auto ui = mUIRef->getUI(BACKGROUND_UI);
        if (ui != nullptr) {
          ui->clear();
          setupBgUI(ui);
          setupRendererUI(ui, mBgRendererRef);
          mUIRef->addShaderParamsUI(ui, *(glslParams.get()));
          mUIRef->loadUI(ui, getAppSupportWorkingSessionSettingsPath());
        } }, [this](ci::Exception exc) { CI_LOG_E(string(BACKGROUND_UI) + " ERROR: " + string(exc.what())); });
    mBgRendererRef->setup();
}

void Transform::updateBg()
{
    mBgRendererRef->update();
}

void Transform::drawBg()
{
    gl::setMatricesWindow(mOutputWindowRef->getSize());
    gl::clear(mBgColor);
    auto glslProgRef = mBgRendererRef->getGlslProgRef();
    if (glslProgRef && mBgRendererRef->mDraw) {
        setDefaultUniforms(glslProgRef);
        mOutputFboRef->getColorTexture()->bind(0);
        glslProgRef->uniform("iScreenTexture", 0);
        mBgRendererRef->draw();
    }
}

void Transform::drawBg(const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll)
{
    vec2 size = mOutputWindowRef->getSize();
    gl::setMatricesWindow(size);
    auto glslProgRef = mBgRendererRef->getGlslProgRef();
    if (glslProgRef) {
        setDefaultUniforms(glslProgRef);
        mOutputFboRef->getColorTexture()->bind(0);
        glslProgRef->uniform("iScreenTexture", 0);
        auto batch = gl::Batch::create(
            geom::Rect(Rectf(0.0f, 0.0f, size.x, size.y)).texCoords(ul, ur, lr, ll),
            glslProgRef);
        batch->draw();
    }
}

void Transform::setupOsc()
{
    mReceiverRef = nullptr;
    mReceiverRef = ReceiverRef(new Receiver(mOscPort));
    mReceiverRef->setListener("/*",
                              [this](const osc::Message &msg) {
                                  string address = msg.getAddress().substr(1);
                                  string typeTag = msg.getTypeTagString();
                                  vector<string> keys = split(address, "/");
                                  int numKeys = int(keys.size());
                                  if (numKeys > 1) {
                                      auto ui = mUIRef->getUI(keys[0]);
                                      if (ui) {
                                          string subkey = keys[1];
                                          auto view = ui->getSubView(subkey);
                                          if (view == nullptr) {
                                              std::transform(subkey.begin(), subkey.end(), subkey.begin(), ::toupper);
                                              view = ui->getSubView(subkey);
                                          }
                                          if (view) {
                                              string type = view->getType();
                                              if (type == "Slideri" && typeTag == "f") {
                                                  Slideri *widget = static_cast<Slideri *>(view.get());
                                                  widget->setValue(lmap<float>(msg.getArgFloat(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              } else if (type == "Sliderf" && typeTag == "f") {
                                                  Sliderf *widget = static_cast<Sliderf *>(view.get());
                                                  widget->setValue(lmap<float>(msg.getArgFloat(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              } else if (type == "Toggle") {
                                                  Toggle *toggle = static_cast<Toggle *>(view.get());
                                                  if (typeTag == "f") {
                                                      toggle->setValue(msg.getArgFloat(0));
                                                  } else if (typeTag == "b") {
                                                      toggle->setValue(msg.getArgBool(0));
                                                  }
                                              } else if (type == "Button") {
                                                  Button *button = static_cast<Button *>(view.get());
                                                  if (typeTag == "f") {
                                                      button->setValue(msg.getArgFloat(0));
                                                  } else if (typeTag == "b") {
                                                      button->setValue(msg.getArgBool(0));
                                                  }
                                              } else if (type == "XYPad") {
                                                  XYPad *widget = static_cast<XYPad *>(view.get());
                                                  if (typeTag == "ff") {
                                                      vec2 min = widget->getMin();
                                                      vec2 max = widget->getMax();
                                                      float x = lmap<float>(msg.getArgFloat(0), 0, 1, min.x, max.x);
                                                      float y = lmap<float>(msg.getArgFloat(1), 0, 1, min.y, max.y);
                                                      widget->setValue(vec2(x, y));
                                                  }
                                              } else if (type == "Sliderd" && typeTag == "f") {
                                                  Sliderd *widget = static_cast<Sliderd *>(view.get());
                                                  widget->setValue(lmap<double>(msg.getArgDouble(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              } else if (type == "MultiSlider" && typeTag == "f" && numKeys > 2) {
                                                  MultiSlider *widget = static_cast<MultiSlider *>(view.get());
                                                  int index = stoi(keys[2]);
                                                  if ((index - 1) < int(widget->getSubViews().size())) {
                                                      string key = subkey;
                                                      switch (index) {
                                                          case 1:
                                                              key += "-X";
                                                              break;
                                                          case 2:
                                                              key += "-Y";
                                                              break;
                                                          case 3:
                                                              key += "-Z";
                                                              break;
                                                          case 4:
                                                              key += "-W";
                                                              break;
                                                          default:
                                                              break;
                                                      }
                                                      widget->setValue(key, lmap<float>(msg.getArgFloat(0), 0.0, 1.0, widget->getMin(key), widget->getMax(key)));
                                                  }
                                              } else if (type == "Dialeri" && typeTag == "f") {
                                                  Dialeri *widget = static_cast<Dialeri *>(view.get());
                                                  widget->setValue(lmap<float>(msg.getArgFloat(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              } else if (type == "Dialerf" && typeTag == "f") {
                                                  Dialerf *widget = static_cast<Dialerf *>(view.get());
                                                  widget->setValue(lmap<float>(msg.getArgFloat(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              } else if (type == "Dialerd" && typeTag == "f") {
                                                  Dialerd *widget = static_cast<Dialerd *>(view.get());
                                                  widget->setValue(lmap<double>(msg.getArgDouble(0), 0.0, 1.0, widget->getMin(), widget->getMax()));
                                              }
                                              view->trigger();
                                          }
                                      }
                                  }
                              });

    try {
        mReceiverRef->bind();
    } catch (const osc::Exception &ex) {
        CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
    }
#if USE_UDP
    mReceiverRef->listen(
        [](asio::error_code error, protocol::endpoint endpoint) -> bool {
            if (error) {
                CI_LOG_E("Error Listening: " << error.message() << " val: " << error.value() << " endpoint: " << endpoint);
                return false;
            } else
                return true;
        });
#endif
}

CINDER_APP(Transform, RendererGl(RendererGl::Options().msaa(16)), Transform::prepareSettings);
