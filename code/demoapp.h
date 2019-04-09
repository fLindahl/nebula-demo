#pragma once
//------------------------------------------------------------------------------
/**
    @class Demo::DemoApplication
    
    (C) 2019 Individual contributors, see AUTHORS file
*/
#include "appgame/gameapplication.h"
#include "renderutil/mayacamerautil.h"
#include "renderutil/freecamerautil.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/stage.h"
#include "graphics/cameracontext.h"
#include "resources/resourcemanager.h"
#include "models/modelcontext.h"
#include "input/inputserver.h"
#include "io/ioserver.h"
#include "audiodevice.h"
#include "basegamefeature/basegamefeatureunit.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "audiofeature/audiofeatureunit.h"
#include "scripting/scriptserver.h"
#include "nsharp/monoserver.h"


namespace Demo
{

class DemoApplication : public App::GameApplication
{
public:
    /// Constructor
	DemoApplication();
    /// Destructor
    ~DemoApplication();

    /// Open
    bool Open();
    /// Close
    void Close();
    /// Run
    void Run();
	/// Setup game features
	void SetupGameFeatures();
	/// Clean up all game features
	void CleanupGameFeatures();

protected:
    void RenderUI();
    void UpdateCamera();
    void ResetCamera();
    void ToMaya();
    void ToFree();
    void Browse();

	Ptr<Audio::AudioDevice> audioDevice;
	Ptr<Scripting::MonoServer> monoserver;

	Ptr<BaseGameFeature::BaseGameFeatureUnit> gameFeature;
	Ptr<GraphicsFeature::GraphicsFeatureUnit> graphicsFeature;
	Ptr<AudioFeature::AudioFeatureUnit> audioFeature;

	// Render
	Ptr<Graphics::GraphicsServer> gfxServer;
	Ptr<Resources::ResourceManager> resMgr;
	Ptr<Input::InputServer> inputServer;
	Ptr<IO::IoServer> ioServer;

	CoreGraphics::WindowId wnd;
	Ptr<Graphics::View> view;
	Ptr<Graphics::View> pickingView;
	Ptr<Graphics::Stage> stage;


	Graphics::GraphicsEntityId cam;
    Graphics::GraphicsEntityId entity;

	Graphics::GraphicsEntityId ground;
	Graphics::GraphicsEntityId globalLight;
	Graphics::GraphicsEntityId pointLights[3];
	Graphics::GraphicsEntityId spotLights[3];
    IndexT frameIndex = -1;

    Util::Array<Util::String> folders;
    int selectedFolder = 0;
    Util::Array<Util::String> files;
    int selectedFile = 0;

    Math::transform44 transform;

    bool renderDebug = false;
    int cameraMode = 0;
    float zoomIn = 0.0f;
    float zoomOut = 0.0f;
    Math::float2 panning{ 0.0f,0.0f };
    Math::float2 orbiting{ 0.0f,0.0f };    
    RenderUtil::MayaCameraUtil mayaCameraUtil;
    RenderUtil::FreeCameraUtil freeCamUtil;        
    Math::point defaultViewPoint;
    Util::Array<Graphics::GraphicsEntityId> entities;
};

}