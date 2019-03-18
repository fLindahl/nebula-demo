#include "stdneb.h"
#include "vertextool.h"
#include "models/modelcontext.h"
#include "input/inputserver.h"
#include "coregraphics/memorymeshpool.h"
#include "graphics/graphicsserver.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "graphics/cameracontext.h"
#include "renderutil/mouserayutil.h"
#include "math/matrix44.h"
#include "math/line.h"
#include "models/nodes/primitivenode.h"
#include "dynui/im3d/im3d.h"
#include "imgui.h"

using namespace Graphics;
using namespace Models;

namespace Tools
{

VertexTool::VertexTool()
{
	timer.Start();
}

VertexTool::~VertexTool()
{
	timer.Stop();
}

void VertexTool::Update()
{
	Ptr<Input::InputServer> inputServer = Input::InputServer::Instance();
	const Ptr<Input::Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
	const Ptr<Input::Mouse>& mouse = inputServer->GetDefaultMouse();

	Timing::Time start = timer.GetTime();
	if (this->mapped)
	{
		Math::line line;
		if (!keyboard->KeyPressed(Input::Key::Code::LeftMenu) && mouse->ButtonPressed(Input::MouseButton::Code::RightButton))
		{
			line = RenderUtil::MouseRayUtil::ComputeWorldMouseRay(
				mouse->GetScreenPosition(),
				9999999.0f,
				Math::matrix44::inverse(CameraContext::GetTransform(this->cam)),
				Math::matrix44::inverse(CameraContext::GetProjection(this->cam)),
				CameraContext::GetSettings(this->cam).GetZNear()
			);
		}

		float closestDistance = 999999999999.0f;

		const Util::Array<Models::ModelNode::Instance*>& nodes = ModelContext::GetModelNodeInstances(this->entity);
		const auto& nodeTypes = ModelContext::GetModelNodeTypes(this->entity);

		for (IndexT j = 0; j < numVertices; j++)
		{
			IndexT index = baseVertex + positionOffset + (j * vertexSize);
			float* v = (float*)&(((ubyte*)vbo)[index]);
			Math::point vertex(v[0], v[1], v[2]);
					
			if (!keyboard->KeyPressed(Input::Key::Code::LeftMenu) && mouse->ButtonPressed(Input::MouseButton::Code::RightButton))
			{
				float dist = line.distance(vertex);
				if (dist < closestDistance)
				{

					closestDistance = dist;
					hoveredIndex = index;
				}
			}

			const Im3d::Color color = Im3d::Color_Green;
			float size = 10.0f;

			Im3d::DrawPoint(Im3d::Vec3(vertex[0], vertex[1], vertex[2]), size, color);
		}

		// Draw hovered index last
		Im3d::Color color = Im3d::Color_Red;
		float size = 15.0f;
		float* vertex;
		vertex = ((float*)&(((ubyte*)vbo)[hoveredIndex]));
		Im3d::DrawPoint(Im3d::Vec3(vertex[0], vertex[1], vertex[2]), size, color);

		if (Im3d::GizmoTranslation("vertex", vertex, false))
		{
					
		}
	}
	Timing::Time last = timer.GetTime();

	static bool open = true;
	ImGui::Begin("VertexTool", &open);
	ImGui::SetWindowPos({ 0,0 }, ImGuiCond_Once);
	ImGui::SetWindowSize({ 200, 100 }, ImGuiCond_Once);
	ImGui::Text(Util::String::FromFloat2(mouse->GetScreenPosition()).AsCharPtr());
	ImGui::Text("Hovered index: %i", hoveredIndex);
	ImGui::Text("Selected index: %i", selectedIndex);
	ImGui::Text("VertexTool update:  %fms", last - start);
	ImGui::End();
}

void VertexTool::SetGraphicsEntity(Graphics::GraphicsEntityId entity)
{
	this->entity = entity;

	if (mapped)
	{
		CoreGraphics::IndexBufferUnmap(this->ib);
		CoreGraphics::VertexBufferUnmap(this->vb);
		mapped = false;
	}

	const Util::Array<Models::ModelNode::Instance*>& nodes = ModelContext::GetModelNodeInstances(this->entity);
	const auto& nodeTypes = ModelContext::GetModelNodeTypes(this->entity);

	IndexT n;
	for (n = 0; n < nodes.Size(); ++n)
	{
		if (nodeTypes[n] == Models::NodeType::PrimitiveNodeType)
		{
			break;
		}
	}

	auto primitive = static_cast<Models::PrimitiveNode*>(nodes[n]->node);
	auto meshId = primitive->GetMeshId();
	auto groupIdx = primitive->GetPrimitiveGroupIndex();
	CoreGraphics::meshPool->BeginGet();
	auto const& primGroups = CoreGraphics::MeshGetPrimitiveGroups(meshId);

	this->baseIndex = primGroups[groupIdx].GetBaseIndex();
	this->baseVertex = primGroups[groupIdx].GetBaseVertex();
	this->numIndices = primGroups[groupIdx].GetNumIndices();
	this->numVertices = primGroups[groupIdx].GetNumVertices();
	auto vertexLayoutId = primGroups[groupIdx].GetVertexLayout();
	this->vertexSize = CoreGraphics::VertexLayoutGetSize(vertexLayoutId);

	this->ib = CoreGraphics::MeshGetIndexBuffer(meshId);
	this->vb = CoreGraphics::MeshGetVertexBuffer(meshId, 0);

	CoreGraphics::meshPool->EndGet();

	auto vcs = CoreGraphics::VertexLayoutGetComponents(vertexLayoutId);
	positionOffset = 0;
	for (auto const& vc : vcs)
	{
		if (vc.GetSemanticName() == CoreGraphics::VertexComponent::Position)
		{
			positionOffset = vc.GetByteOffset();
			break;
		}
	}

	auto indexType = CoreGraphics::IndexBufferGetType(ib);
	vbo = CoreGraphics::VertexBufferMap(vb, CoreGraphics::GpuBufferTypes::MapReadWrite);
	ibo = CoreGraphics::IndexBufferMap(ib, CoreGraphics::GpuBufferTypes::MapReadWrite);
	this->mapped = true;
}

void VertexTool::SetCameraEntity(Graphics::GraphicsEntityId cam)
{
	this->cam = cam;
}

void VertexTool::CreateBrush()
{
	auto id = Graphics::CreateEntity();
	this->brushes.Append(id);
	// @todo	Create a new box model to modify.
}



}