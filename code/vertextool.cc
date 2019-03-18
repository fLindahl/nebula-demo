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
#include "coregraphics/mesh.h"

using namespace Graphics;
using namespace Models;
using namespace CoreGraphics;

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
		line = RenderUtil::MouseRayUtil::ComputeWorldMouseRay(
			mouse->GetScreenPosition(),
			9999999.0f,
			Math::matrix44::inverse(CameraContext::GetTransform(this->cam)),
			Math::matrix44::inverse(CameraContext::GetProjection(this->cam)),
			CameraContext::GetSettings(this->cam).GetZNear()
		);
		
		float closestDistance = 999999999999.0f;

		const Util::Array<Models::ModelNode::Instance*>& nodes = ModelContext::GetModelNodeInstances(this->entity);
		const auto& nodeTypes = ModelContext::GetModelNodeTypes(this->entity);

		for (IndexT j = 0; j < numVertices; j++)
		{
			IndexT index = baseVertex + positionOffset + (j * vertexSize);
			float* v = (float*)&(((ubyte*)vbo)[index]);
			Math::point vertex(v[0], v[1], v[2]);
					
			float dist = line.distance(vertex);
			if (dist < closestDistance)
			{
				closestDistance = dist;
				hoveredIndex = index;
			}

			const Im3d::Color color = Im3d::Color(1.0,1.0,1.0,1.0f);
			float size = 7.5f;

			Im3d::DrawPoint(Im3d::Vec3(vertex[0], vertex[1], vertex[2]), size, color);
		}

		if (!keyboard->KeyPressed(Input::Key::Code::LeftMenu) && mouse->ButtonPressed(Input::MouseButton::Code::RightButton))
		{
			selectedIndex = hoveredIndex;
		}

		// Draw hovered index last
		Im3d::Color color = Im3d::Color(0.0,0.8f,0.0f,1.0f);
		float size = 10.0f;
		float* vertex;
		vertex = ((float*)&(((ubyte*)vbo)[hoveredIndex]));
		Im3d::DrawPoint(Im3d::Vec3(vertex[0], vertex[1], vertex[2]), size, color);


		color = Im3d::Color_Red;
		size = 15.0f;
		vertex = ((float*)&(((ubyte*)vbo)[selectedIndex]));
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
	static uint meshUniqueIdentifier = 0;
	meshUniqueIdentifier++;
	Util::String meshName = "BRUSH";
	meshName.AppendInt(meshUniqueIdentifier);

	const auto PackComponent = [](float x, float y, float z, float w)
	{
		int xBits = (int)x;
		int yBits = (int)y;
		int zBits = (int)z;
		int wBits = (int)w;
		return ((wBits << 24) & 0xFF000000) | ((zBits << 16) & 0x00FF0000) | ((yBits << 8) & 0x0000FF00) | (xBits & 0x000000FF);
	};

	float x = 0 * 0.5f * 255.0f;
	float y = 1 * 0.5f * 255.0f;
	float z = 0 * 0.5f * 255.0f;
	float w = 0 * 0.5f * 255.0f;
	int normPacked = PackComponent(x, y, z, w);

	x = 0 * 0.5f * 255.0f;
	y = 0 * 0.5f * 255.0f;
	z = 1 * 0.5f * 255.0f;
	w = 0 * 0.5f * 255.0f;
	int tangentPacked = PackComponent(x, y, z, w);

	float width = 1.0f;
	float height = 1.0f;
	float depth = 1.0f;
	float w2 = width / 2.0f;
	float h2 = height / 2.0f;
	float d2 = depth / 2.0f;

	float vertex[] = { 
		-w2, -h2, d2, 0, 0,
		 w2,  h2, d2, 0, 0,
		-w2, -h2, d2, 0, 0,
		 w2, -h2, d2, 0, 0,
		-w2, -h2, -d2, 0, 0,
		 w2,  h2, -d2, 0, 0,
		-w2, -h2, -d2, 0, 0,
		 w2, -h2, -d2, 0, 0,
	};
	*(int*)&vertex[3] = normPacked;
	*(int*)&vertex[4] = tangentPacked;
	*(int*)&vertex[8] = normPacked;
	*(int*)&vertex[9] = tangentPacked;
	*(int*)&vertex[13] = normPacked;
	*(int*)&vertex[14] = tangentPacked;
	*(int*)&vertex[18] = normPacked;
	*(int*)&vertex[19] = tangentPacked;
	*(int*)&vertex[23] = normPacked;
	*(int*)&vertex[24] = tangentPacked;
	*(int*)&vertex[28] = normPacked;
	*(int*)&vertex[29] = tangentPacked;
	*(int*)&vertex[33] = normPacked;
	*(int*)&vertex[34] = tangentPacked;
	*(int*)&vertex[38] = normPacked;
	*(int*)&vertex[39] = tangentPacked;

	// setup mesh
	Util::Array<VertexComponent> vertexComponents;
	vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3, 0));
	vertexComponents.Append(VertexComponent(VertexComponent::Normal, 0, VertexComponent::Byte4N, 0));
	vertexComponents.Append(VertexComponent(VertexComponent::Tangent, 0, VertexComponent::Byte4N, 0));

	VertexBufferCreateInfo vboInfo =
	{
		meshName + "_VBO",
		"BRUSHES"_atm,
		GpuBufferTypes::AccessReadWrite, GpuBufferTypes::UsageDynamic, GpuBufferTypes::SyncingPersistent,
		8, vertexComponents,
		vertex, sizeof(vertex)
	};
	VertexBufferId vbo = CreateVertexBuffer(vboInfo);
	if (vbo != VertexBufferId::Invalid())
	{
		n_error("VertexTool::CreateBrush: Failed to setup default mesh");
	}

	uint indices[] = { 
		0, 1, 2,
		2, 1, 3,
		1, 5, 3,
		3, 5, 7,
		5, 4, 7,
		4, 6, 7,
		4, 0, 6,
		6, 0, 2,
		0, 4, 5,
		0, 5, 1,
		2, 3, 6,
		3, 7, 6
	};
	IndexBufferCreateInfo iboInfo = {
		meshName + "_IBO",
		"BRUSHES"_atm,
		GpuBufferTypes::AccessReadWrite, GpuBufferTypes::UsageDynamic, GpuBufferTypes::SyncingPersistent,
		IndexType::Index32,	36, indices, sizeof(indices)
	};
	IndexBufferId ibo = CreateIndexBuffer(iboInfo);
	if (ibo != IndexBufferId::Invalid())
	{
		n_error("VertexTool::CreateBrush: Failed to setup default mesh");
	}
	VertexLayoutCreateInfo vloInfo = {
		vertexComponents
	};
	VertexLayoutId vlo = CreateVertexLayout(vloInfo);

	PrimitiveGroup group;
	group.SetBaseIndex(0);
	group.SetBaseVertex(0);
	group.SetNumIndices(36);
	group.SetNumVertices(8);
	group.SetVertexLayout(VertexBufferGetLayout(vbo));
	Util::Array<PrimitiveGroup> groups;
	groups.Append(group);

	MeshCreateInfo info = {
		meshName,
		"BRUSHES"_atm,
		{ { vbo, 0 } }, ibo, vlo, CoreGraphics::PrimitiveTopology::TriangleList, groups
	};
	
	auto meshId = CreateMesh(info);

	auto id = Graphics::CreateEntity();

	this->brushes.Append(id);
}



}