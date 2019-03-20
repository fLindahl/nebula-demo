#pragma once
#include "util/array.h"
#include "graphics/graphicsentity.h"
#include "coregraphics/memoryindexbufferpool.h"
#include "coregraphics/memoryvertexbufferpool.h"
#include "timing/timer.h"

namespace Tools
{

class VertexTool
{
public:
	VertexTool();
	~VertexTool();

	void Update();

	void SetGraphicsEntity(Graphics::GraphicsEntityId entity);
	void SetCameraEntity(Graphics::GraphicsEntityId cam);

	void CreateBrush();

private:
	Graphics::GraphicsEntityId entity;
	Graphics::GraphicsEntityId cam;
	IndexT hoveredIndex = 0;
	Util::Array<IndexT> selectedIndex = { 0 };

	Math::float2 selectionStart;
	Math::float2 selectionEnd;

	Util::Array<Graphics::GraphicsEntityId> brushes;

	//--------
	void* vbo;
	void* ibo;
	CoreGraphics::VertexBufferId vb;
	CoreGraphics::IndexBufferId ib;

	IndexT baseIndex;
	IndexT baseVertex;
	SizeT numIndices;
	SizeT numVertices;
	ubyte positionOffset;
	SizeT vertexSize;

	bool mapped = false;
	
	//----------
	Timing::Timer timer;
};

}