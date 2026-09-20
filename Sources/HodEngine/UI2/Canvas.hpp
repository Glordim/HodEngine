#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;

	class HOD_UI2_API Canvas final
	{
	public:

		Canvas();
		Canvas(const Canvas&) = delete;
		Canvas(Canvas&&) = delete;
		~Canvas();

		Canvas& operator=(const Canvas&) = delete;
		Canvas& operator=(Canvas&&) = delete;

		void	AddChild(Node* node);
		void	AddChild(Node* node, LayoutParams* layoutParams);
		void	RemoveChild(Node* node);

		Node*	GetRootNode() const;

		// Drives the top-down Measure/Arrange pass over the whole node tree, using _designResolution
		// as the root's available size. The only place Node::Measure/Node::Arrange are called from.
		void	UpdateLayout();

		// Queues the whole node tree's drawing into renderView's queueType queue. Layout must be up to
		// date (UpdateLayout()). baseMatrix maps canvas space into that queue's space: identity when canvas
		// units are the queue's units (editor viewport), ComputeFitMatrix() for in-game UI on the UI queue.
		void	PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix = Matrix4::Identity);

		// Uniform scale making the design resolution fit (letterboxed) in a render of the given resolution.
		Matrix4	ComputeFitMatrix(const Vector2& resolution) const;

		bool	SerializeInDocument(DocumentNode& documentNode);
		bool	DeserializeFromDocument(const DocumentNode& documentNode);

	private:

		Node*	_rootNode;
		Vector2 _designResolution = Vector2(1920.0f, 1080.0f);
	};
}
