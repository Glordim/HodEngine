#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Canvas.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Math/Rect.hpp>

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
}
