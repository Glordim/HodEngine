#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Canvas.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Math/Rect.hpp>

#include <algorithm>

namespace hod::inline ui2
{
	Canvas::Canvas()
	{
		_rootNode = DefaultAllocator::GetInstance().New<Node>();
		_rootNode->SetDesiredSize(_designResolution);
	}

	Canvas::~Canvas()
	{
		DefaultAllocator::GetInstance().Delete(_rootNode);
	}

	void Canvas::AddChild(Node* node)
	{
		_rootNode->AddChild(node);
	}

	void Canvas::AddChild(Node* node, LayoutParams* layoutParams)
	{
		_rootNode->AddChild(node, layoutParams);
	}

	Node* Canvas::GetRootNode() const
	{
		return _rootNode;
	}

	bool Canvas::SerializeInDocument(DocumentNode& documentNode)
	{
		return _rootNode->SerializeInDocument(documentNode);
	}

	bool Canvas::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		return _rootNode->DeserializeFromDocument(documentNode);
	}

	void Canvas::RemoveChild(Node* node)
	{
		_rootNode->RemoveChild(node);
	}

	void Canvas::UpdateLayout()
	{
		_rootNode->Measure(_designResolution);

		Rect finalRect;
		finalRect._position = _designResolution * -0.5f;
		finalRect._size = _designResolution;
		_rootNode->Arrange(finalRect);
	}

	void Canvas::PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix)
	{
		DrawContext drawContext(renderView, queueType, baseMatrix);
		_rootNode->PushRenderCommand(drawContext);
	}

	Matrix4 Canvas::ComputeFitMatrix(const Vector2& resolution) const
	{
		if (_designResolution.GetX() <= 0.0f || _designResolution.GetY() <= 0.0f)
		{
			return Matrix4::Identity;
		}

		float scale = std::min(resolution.GetX() / _designResolution.GetX(), resolution.GetY() / _designResolution.GetY());
		return Matrix4::Scale(Vector2(scale, scale));
	}
}
