//------------------------------------------------------------------------------
// demoapp.cc
// (C) 2019 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/refcounted.h"
#include "timing/timer.h"
#include "io/console.h"
#include "visibility/visibilitycontext.h"
#include "models/streammodelpool.h"
#include "models/modelcontext.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "demoapp.h"
#include "math/vector.h"
#include "math/point.h"
#include "dynui/imguicontext.h"
#include "lighting/lightcontext.h"
#include "imgui.h"
#include "dynui/im3d/im3dcontext.h"
#include "dynui/im3d/im3d.h"
#include "scripting/mono/monoserver.h"
#include "vertextool.h"

#ifdef __WIN32__
#include <shellapi.h>
#elif __LINUX__

#endif


using namespace Timing;
using namespace Graphics;
using namespace Visibility;
using namespace Models;

namespace Demo
{

//------------------------------------------------------------------------------
/**
*/
DemoApplication::DemoApplication()
{
    this->SetAppTitle("Demo Application");
    this->SetCompanyName("Nebula");
}

//------------------------------------------------------------------------------
/**
*/
DemoApplication::~DemoApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
DemoApplication::Open()
{
    if (this->GameApplication::Open())
    {
		this->gfxServer = GraphicsServer::Create();
        this->resMgr = Resources::ResourceManager::Create();
        this->inputServer = Input::InputServer::Create();

        this->resMgr->Open();
        this->inputServer->Open();
        this->gfxServer->Open();

		this->scriptserver = Scripting::MonoServer::Create();
		this->scriptserver->Open();

        SizeT width = this->GetCmdLineArgs().GetInt("-w", 800);
        SizeT height = this->GetCmdLineArgs().GetInt("-h", 600);
        
        CoreGraphics::WindowCreateInfo wndInfo =
        {
            CoreGraphics::DisplayMode{ 100, 100, width, height },
            this->GetAppTitle(), "", CoreGraphics::AntiAliasQuality::None, true, true, false
        };
        this->wnd = CreateWindow(wndInfo);

        // create contexts, this could and should be bundled together
        CameraContext::Create();
        ModelContext::Create();
        ObserverContext::Create();
        ObservableContext::Create();
		Lighting::LightContext::Create();
        Dynui::ImguiContext::Create();
        Im3d::Im3dContext::Create();

        Im3d::Im3dContext::SetGridStatus(true);
        Im3d::Im3dContext::SetGridSize(1.0f, 25);
        Im3d::Im3dContext::SetGridColor(Math::float4(0.2f, 0.2f, 0.2f, 0.8f));

        this->view = gfxServer->CreateView("mainview", "frame:vkdebug.json");
        this->stage = gfxServer->CreateStage("stage1", true);
        this->cam = Graphics::CreateEntity();
        CameraContext::RegisterEntity(this->cam);
        CameraContext::SetupProjectionFov(this->cam, width / (float)height, 45.f, 0.01f, 1000.0f);

		this->defaultViewPoint = Math::point(15.0f, 15.0f, -15.0f);
        this->ResetCamera();
        CameraContext::SetTransform(this->cam, this->mayaCameraUtil.GetCameraTransform());

        this->view->SetCamera(this->cam);
        this->view->SetStage(this->stage);

		// register visibility system
		ObserverContext::CreateBruteforceSystem({});

		this->globalLight = Graphics::CreateEntity();
		Lighting::LightContext::RegisterEntity(this->globalLight);
		Lighting::LightContext::SetupGlobalLight(this->globalLight, Math::float4(1, 1, 1, 0), 1.0f, Math::float4(0, 0, 0, 0), Math::float4(0, 0, 0, 0), 0.0f, Math::vector(1, 1, 1), false);

		this->pointLights[0] = Graphics::CreateEntity();
		Lighting::LightContext::RegisterEntity(this->pointLights[0]);
		Lighting::LightContext::SetupPointLight(this->pointLights[0], Math::float4(1, 0, 0, 1), 10.0f, Math::matrix44::translation(0, 0, -10), 10.0f, false);

		this->pointLights[1] = Graphics::CreateEntity();
		Lighting::LightContext::RegisterEntity(this->pointLights[1]);
		Lighting::LightContext::SetupPointLight(this->pointLights[1], Math::float4(0, 1, 0, 1), 10.0f, Math::matrix44::translation(-10, 0, -10), 10.0f, false);

		this->pointLights[2] = Graphics::CreateEntity();
		Lighting::LightContext::RegisterEntity(this->pointLights[2]);
		Lighting::LightContext::SetupPointLight(this->pointLights[2], Math::float4(0, 0, 1, 1), 10.0f, Math::matrix44::translation(-10, 0, 0), 10.0f, false);

		for (int i = 0; i < 3; i++)
		{
			this->entities.Append(this->pointLights[i]);
		}

		{
			this->spotLights[0] = Graphics::CreateEntity();
			Lighting::LightContext::RegisterEntity(this->spotLights[0]);
			Math::matrix44 spotLightMatrix;
			spotLightMatrix.scale(Math::vector(30, 30, 40));
			spotLightMatrix = Math::matrix44::multiply(spotLightMatrix, Math::matrix44::rotationyawpitchroll(0, Math::n_deg2rad(-55), 0));
			spotLightMatrix.set_position(Math::point(0, 5, 2));
			Lighting::LightContext::SetupSpotLight(this->spotLights[0], Math::float4(1, 1, 0, 1), 1.0f, spotLightMatrix, false);
		}

		{
			this->spotLights[1] = Graphics::CreateEntity();
			Lighting::LightContext::RegisterEntity(this->spotLights[1]);
			Math::matrix44 spotLightMatrix;
			spotLightMatrix.scale(Math::vector(30, 30, 40));
			spotLightMatrix = Math::matrix44::multiply(spotLightMatrix, Math::matrix44::rotationyawpitchroll(Math::n_deg2rad(60), Math::n_deg2rad(-55), 0));
			spotLightMatrix.set_position(Math::point(2, 5, 0));
			Lighting::LightContext::SetupSpotLight(this->spotLights[1], Math::float4(0, 1, 1, 1), 1.0f, spotLightMatrix, false);
		}

		{
			this->spotLights[2] = Graphics::CreateEntity();
			Lighting::LightContext::RegisterEntity(this->spotLights[2]);
			Math::matrix44 spotLightMatrix;
			spotLightMatrix.scale(Math::vector(30, 30, 40));
			spotLightMatrix = Math::matrix44::multiply(spotLightMatrix, Math::matrix44::rotationyawpitchroll(Math::n_deg2rad(120), Math::n_deg2rad(-55), 0));
			spotLightMatrix.set_position(Math::point(2, 5, 2));
			Lighting::LightContext::SetupSpotLight(this->spotLights[2], Math::float4(1, 0, 1, 1), 1.0f, spotLightMatrix, false);
		}

		this->ground = Graphics::CreateEntity();
		ModelContext::RegisterEntity(this->ground);
		ModelContext::Setup(this->ground, "mdl:environment/Groundplane.n3", "Viewer");
		ModelContext::SetTransform(this->ground, Math::matrix44::translation(Math::float4(0, 0, 0, 1)));
		this->entities.Append(this->ground);

        this->entity = Graphics::CreateEntity();
        ModelContext::RegisterEntity(this->entity);
        ModelContext::Setup(this->entity, "mdl:Buildings/castle_tower.n3", "Viewer");
        ModelContext::SetTransform(this->entity, Math::matrix44::translation(Math::float4(0, 0, 0, 1)));
        this->entities.Append(this->entity);

        ObservableContext::RegisterEntity(this->entity);
        ObservableContext::Setup(this->entity, VisibilityEntityType::Model);
		ObservableContext::RegisterEntity(this->ground);
		ObservableContext::Setup(this->ground, VisibilityEntityType::Model);
        ObserverContext::RegisterEntity(this->cam);
        ObserverContext::Setup(this->cam, VisibilityEntityType::Camera);

		Util::Array<Graphics::GraphicsEntityId> models;
		ModelContext::BeginBulkRegister();
		ObservableContext::BeginBulkRegister();
		static const int NumModels = 1;
		for (IndexT i = -NumModels; i < NumModels; i++)
		{
			for (IndexT j = -NumModels; j < NumModels; j++)
			{
				Graphics::GraphicsEntityId ent = Graphics::CreateEntity();
                this->entities.Append(ent);

				// create model and move it to the front
				ModelContext::RegisterEntity(ent);
				ModelContext::Setup(ent, "mdl:Buildings/castle_tower.n3", "NotA");
				ModelContext::SetTransform(ent, Math::matrix44::translation(Math::float4(i * 10, 0, -j * 10, 1)));

				ObservableContext::RegisterEntity(ent);
				ObservableContext::Setup(ent, VisibilityEntityType::Model);
				models.Append(ent);
			}
		}
		ModelContext::EndBulkRegister();
		ObservableContext::EndBulkRegister();
        this->UpdateCamera();

		// IO::AssignRegistry::Instance()->SetAssign(IO::Assign("scripts", "tool:data/scripts"));

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::Close()
{
    DestroyWindow(this->wnd);
    this->gfxServer->DiscardStage(this->stage);
    this->gfxServer->DiscardView(this->view);

    this->gfxServer->Close();
    this->inputServer->Close();
    this->resMgr->Close();
    this->Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::Run()
{    
    bool run = true;

	Tools::VertexTool vertexTool;
	vertexTool.SetCameraEntity(this->cam);
	
    while (run && !inputServer->IsQuitRequested())
    {                     
        this->inputServer->BeginFrame();
        this->inputServer->OnFrame();

        this->resMgr->Update(this->frameIndex);

		// animate the spotlights
		IndexT i;
		for (i = 0; i < 3; i++)
		{
			Math::matrix44 spotLightTransform;
			Math::scalar scaleFactor = i * 1.5f + 30;
			spotLightTransform.scale(Math::point(scaleFactor, scaleFactor, scaleFactor + 10));
			spotLightTransform = Math::matrix44::multiply(spotLightTransform, Math::matrix44::rotationyawpitchroll(this->gfxServer->GetTime() * 2 * (i + 1) / 3, Math::n_deg2rad(-55), 0));
			spotLightTransform.set_position(Lighting::LightContext::GetTransform(this->spotLights[i]).get_position());
			Lighting::LightContext::SetTransform(this->spotLights[i], spotLightTransform);
		}

        this->gfxServer->BeginFrame();

		this->GameApplication::StepFrame();
        
        // put game code which doesn't need visibility data or animation here
        this->gfxServer->BeforeViews();
        this->RenderUI();
		
		if (this->inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::O))
		{
			Models::ModelId model = ModelContext::GetModel(this->entity);
			if (modelPool->GetState(model) == Resources::Resource::State::Loaded)
			{
				vertexTool.SetGraphicsEntity(this->entity);
			}
		}
		vertexTool.Update();

        if (this->renderDebug)
        {
            this->gfxServer->RenderDebug(0);
        }
        
		// put game code which need visibility data here

        this->gfxServer->RenderViews();

        // put game code which needs rendering to be done (animation etc) here

        this->gfxServer->EndViews();

        
        // do stuff after rendering is done

        this->gfxServer->EndFrame();

        // force wait immediately
        WindowPresent(wnd, frameIndex);
        if (this->inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::Escape)) run = false;        
                
        if(this->inputServer->GetDefaultKeyboard()->KeyPressed(Input::Key::LeftMenu))
            this->UpdateCamera();

		if (this->inputServer->GetDefaultKeyboard()->KeyDown(Input::Key::F1))
		{
			// Open browser with debug page.
			Util::String url = "http://localhost:";
			url.AppendInt(this->defaultTcpPort);
#ifdef __WIN32__
			ShellExecute(0, 0, url.AsCharPtr(), 0, 0, SW_SHOW);
#elif __LINUX__
			Util::String shellCommand = "open ";
			shellCommand.Append(url);
			system(shellCommand.AsCharPtr());
#else
			n_printf("Cannot open browser. URL is %s\n", url.AsCharPtr());
#endif
		}

        frameIndex++;             
        this->inputServer->EndFrame();

    }
}

//------------------------------------------------------------------------------
/**
*/
void
DemoApplication::SetupGameFeatures()
{
	this->gameFeature = BaseGameFeature::BaseGameFeatureUnit::Create();
	this->graphicsFeature = GraphicsFeature::GraphicsFeatureUnit::Create();
	this->audioFeature = AudioFeature::AudioFeatureUnit::Create();
	this->gameServer->AttachGameFeature(gameFeature);
	this->gameServer->AttachGameFeature(graphicsFeature);
	this->gameServer->AttachGameFeature(audioFeature);
}

//------------------------------------------------------------------------------
/**
*/
void
DemoApplication::CleanupGameFeatures()
{
	this->gameServer->RemoveGameFeature(gameFeature);
	this->gameFeature->CleanupWorld();
	this->gameFeature->Release();
	this->gameFeature = nullptr;

	this->gameServer->RemoveGameFeature(graphicsFeature);
	this->graphicsFeature->Release();
	this->graphicsFeature = nullptr;

	this->gameServer->RemoveGameFeature(audioFeature);
	this->audioFeature->Release();
	this->audioFeature = nullptr;
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::RenderUI()
{
    ImGui::Begin("Viewer", nullptr, 0);
	ImGui::SetWindowSize(ImVec2(240, 400), ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Camera mode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::RadioButton("Maya", &this->cameraMode, 0))this->ToMaya();
        ImGui::SameLine();
        if (ImGui::RadioButton("Free", &this->cameraMode, 1))this->ToFree();
        ImGui::SameLine();
        if (ImGui::Button("Reset")) this->ResetCamera();
    }
    ImGui::Checkbox("Debug Rendering", &this->renderDebug);
    ImGui::End();
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::UpdateCamera()
{
    const Ptr<Input::Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
    const Ptr<Input::Mouse>& mouse = inputServer->GetDefaultMouse();

    this->mayaCameraUtil.SetOrbitButton(mouse->ButtonPressed(Input::MouseButton::LeftButton));
    this->mayaCameraUtil.SetPanButton(mouse->ButtonPressed(Input::MouseButton::MiddleButton));
    this->mayaCameraUtil.SetZoomButton(mouse->ButtonPressed(Input::MouseButton::RightButton));
    this->mayaCameraUtil.SetZoomInButton(mouse->WheelForward());
    this->mayaCameraUtil.SetZoomOutButton(mouse->WheelBackward());
    this->mayaCameraUtil.SetMouseMovement(mouse->GetMovement());

    // process keyboard input
    Math::float4 pos(0.0f);
    if (keyboard->KeyDown(Input::Key::Space))
    {
        this->mayaCameraUtil.Reset();
    }
    if (keyboard->KeyPressed(Input::Key::Left))
    {
        panning.x() -= 0.1f;
        pos.x() -= 0.1f;
    }
    if (keyboard->KeyPressed(Input::Key::Right))
    {
        panning.x() += 0.1f;
        pos.x() += 0.1f;
    }
    if (keyboard->KeyPressed(Input::Key::Up))
    {
        panning.y() -= 0.1f;
        if (keyboard->KeyPressed(Input::Key::LeftShift))
        {
            pos.y() -= 0.1f;
        }
        else
        {
            pos.z() -= 0.1f;
        }
    }
    if (keyboard->KeyPressed(Input::Key::Down))
    {
        panning.y() += 0.1f;
        if (keyboard->KeyPressed(Input::Key::LeftShift))
        {
            pos.y() += 0.1f;
        }
        else
        {
            pos.z() += 0.1f;
        }
    }


    this->mayaCameraUtil.SetPanning(panning);
    this->mayaCameraUtil.SetOrbiting(orbiting);
    this->mayaCameraUtil.SetZoomIn(zoomIn);
    this->mayaCameraUtil.SetZoomOut(zoomOut);
    this->mayaCameraUtil.Update();

    
    this->freeCamUtil.SetForwardsKey(keyboard->KeyPressed(Input::Key::W));
    this->freeCamUtil.SetBackwardsKey(keyboard->KeyPressed(Input::Key::S));
    this->freeCamUtil.SetRightStrafeKey(keyboard->KeyPressed(Input::Key::D));
    this->freeCamUtil.SetLeftStrafeKey(keyboard->KeyPressed(Input::Key::A));
    this->freeCamUtil.SetUpKey(keyboard->KeyPressed(Input::Key::Q));
    this->freeCamUtil.SetDownKey(keyboard->KeyPressed(Input::Key::E));

    this->freeCamUtil.SetMouseMovement(mouse->GetMovement());
    this->freeCamUtil.SetAccelerateButton(keyboard->KeyPressed(Input::Key::LeftShift));

    this->freeCamUtil.SetRotateButton(mouse->ButtonPressed(Input::MouseButton::LeftButton));
    this->freeCamUtil.Update();
    
    switch (this->cameraMode)
    {
    case 0:
        CameraContext::SetTransform(this->cam, Math::matrix44::inverse(this->mayaCameraUtil.GetCameraTransform()));
        break;
    case 1:
        CameraContext::SetTransform(this->cam, Math::matrix44::inverse(this->freeCamUtil.GetTransform()));
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::ResetCamera()
{
    this->freeCamUtil.Setup(this->defaultViewPoint, Math::float4::normalize(this->defaultViewPoint));
    this->freeCamUtil.Update();
    this->mayaCameraUtil.Setup(Math::point(0.0f, 0.0f, 0.0f), this->defaultViewPoint, Math::vector(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::ToMaya()
{
    this->mayaCameraUtil.Setup(this->mayaCameraUtil.GetCenterOfInterest(), this->freeCamUtil.GetTransform().get_position(), Math::vector(0, 1, 0));
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::ToFree()
{
    Math::float4 pos = this->mayaCameraUtil.GetCameraTransform().get_position();
    this->freeCamUtil.Setup(pos, Math::float4::normalize(pos - this->mayaCameraUtil.GetCenterOfInterest()));
}

//------------------------------------------------------------------------------
/**
*/
void 
DemoApplication::Browse()
{
    this->folders = IO::IoServer::Instance()->ListDirectories("mdl:", "*");    
    this->files = IO::IoServer::Instance()->ListFiles("mdl:" + this->folders[this->selectedFolder], "*");
}
}
