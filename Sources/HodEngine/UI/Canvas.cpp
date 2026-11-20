#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/DrawContext.hpp"
#include "HodEngine/UI/UISettings.hpp"

#include <HodEngine/Core/Document/DocumentNode.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Math/Rect.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_ENUM(CanvasScaler::Mode)
	{
		HOD_ENUM_VALUE(Fit);
		HOD_ENUM_VALUE(Expand);
		HOD_ENUM_VALUE(Shrink);
		HOD_ENUM_VALUE(MatchWidth);
		HOD_ENUM_VALUE(MatchHeight);
	}

	DESCRIBE_REFLECTED_CLASS(CanvasScaler)
	{
		HOD_PROPERTY(_designResolution);
		HOD_PROPERTY(_mode);
	}

	/// @brief
	/// @param targetResolution
	/// @param outLayoutSize
	/// @param outScale
	void CanvasScaler::Compute(const Vector2& targetResolution, Vector2& outLayoutSize, float& outScale) const
	{
		outLayoutSize = _designResolution;
		outScale = 1.0f;

		if (_designResolution.GetX() <= 0.0f || _designResolution.GetY() <= 0.0f || targetResolution.GetX() <= 0.0f || targetResolution.GetY() <= 0.0f)
		{
			return;
		}

		float scaleX = targetResolution.GetX() / _designResolution.GetX();
		float scaleY = targetResolution.GetY() / _designResolution.GetY();

		switch (_mode)
		{
			case Mode::Fit: outScale = Min(scaleX, scaleY); return; // The root rect stays the design resolution
			case Mode::Expand: outScale = Min(scaleX, scaleY); break;
			case Mode::Shrink: outScale = Max(scaleX, scaleY); break;
			case Mode::MatchWidth: outScale = scaleX; break;
			case Mode::MatchHeight: outScale = scaleY; break;
		}

		outLayoutSize = targetResolution / outScale;
	}

	Canvas::Canvas()
	{
		_rootNode = DefaultAllocator::GetInstance().New<Node>();
		_rootNode->SetName("Root");
		UpdateScaling();
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

	const CanvasScaler& Canvas::GetScaler() const
	{
		return _scaler;
	}

	void Canvas::SetScaler(const CanvasScaler& scaler)
	{
		if (_scaler != scaler)
		{
			_scaler = scaler;
			UpdateScaling();
		}
	}

	bool Canvas::GetOverrideScaler() const
	{
		return _overrideScaler;
	}

	void Canvas::SetOverrideScaler(bool overrideScaler)
	{
		if (_overrideScaler != overrideScaler)
		{
			_overrideScaler = overrideScaler;
			UpdateScaling();
		}
	}

	const CanvasScaler& Canvas::GetEffectiveScaler() const
	{
		if (_overrideScaler == false)
		{
			const UISettings* uiSettings = UISettings::Get();
			if (uiSettings != nullptr)
			{
				return uiSettings->_canvasScaler;
			}
		}
		return _scaler;
	}

	const Vector2& Canvas::GetTargetResolution() const
	{
		return _targetResolution;
	}

	void Canvas::SetTargetResolution(const Vector2& targetResolution)
	{
		if (_targetResolution != targetResolution)
		{
			_targetResolution = targetResolution;
			UpdateScaling();
		}
	}

	const Vector2& Canvas::GetLayoutSize() const
	{
		return _layoutSize;
	}

	float Canvas::GetScaleFactor() const
	{
		return _scaleFactor;
	}

	/// @brief Recomputes the root rect and scale from the scaler and the target resolution, and has the
	/// whole tree laid out again on the next UpdateLayout() when the root rect changed.
	void Canvas::UpdateScaling()
	{
		Vector2 previousLayoutSize = _layoutSize;

		const CanvasScaler& scaler = GetEffectiveScaler();

		Vector2 targetResolution = (_targetResolution.GetX() > 0.0f && _targetResolution.GetY() > 0.0f) ? _targetResolution : scaler._designResolution;
		scaler.Compute(targetResolution, _layoutSize, _scaleFactor);

		if (_layoutSize != previousLayoutSize)
		{
			_rootNode->MarkMeasureAsDirty();
			_rootNode->MarkArrangeAsDirty();
		}
	}

	bool Canvas::SerializeInDocument(DocumentNode& documentNode)
	{
		if (_rootNode->SerializeInDocument(documentNode) == false)
		{
			return false;
		}

		documentNode.AddChild("OverrideScaler").SetBool(_overrideScaler);
		return Serializer::Serialize(_scaler, documentNode.AddChild("Scaler"));
	}

	bool Canvas::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		if (_rootNode->DeserializeFromDocument(documentNode) == false)
		{
			return false;
		}

		// Absent from prefabs saved before the scaler existed: they keep the defaults.
		const DocumentNode* scalerNode = documentNode.GetChild("Scaler");
		if (scalerNode != nullptr)
		{
			CanvasScaler scaler;
			if (Serializer::Deserialize(scaler, *scalerNode) == false)
			{
				return false;
			}
			SetScaler(scaler);
		}

		// Absent from prefabs saved before the project's UISettings existed: they follow them.
		const DocumentNode* overrideScalerNode = documentNode.GetChild("OverrideScaler");
		SetOverrideScaler(overrideScalerNode != nullptr && overrideScalerNode->GetBool());

		return true;
	}

	void Canvas::RemoveChild(Node* node)
	{
		_rootNode->RemoveChild(node);
	}

	namespace
	{
		void UpdateInteractions(Node* node, float deltaTime)
		{
			if (Interaction* interaction = node->GetInteraction())
			{
				interaction->Update(deltaTime);
			}
			for (Node* child : node->GetChildren())
			{
				UpdateInteractions(child, deltaTime);
			}
		}
	}

	namespace
	{
		void UpdateAnimations(Node* node, float deltaTime)
		{
			node->UpdateAnimations(deltaTime);
			for (Node* child : node->GetChildren())
			{
				UpdateAnimations(child, deltaTime);
			}
		}
	}

	/// @brief
	/// @param deltaTime
	void Canvas::Update(float deltaTime)
	{
		UpdateInteractions(_rootNode, deltaTime);
		// After them: a clip drives its properties over a state transition touching the same ones
		UpdateAnimations(_rootNode, deltaTime);
	}

	void Canvas::UpdateLayout()
	{
		UpdateScaling();

		_rootNode->Measure(_layoutSize);

		Rect finalRect;
		finalRect._position = _layoutSize * -0.5f;
		finalRect._size = _layoutSize;
		_rootNode->Arrange(finalRect);
	}

	void Canvas::PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix, uint32_t firstOrder, uint32_t orderStep,
	                               uint32_t pickingId)
	{
		DrawContext drawContext(renderView, queueType, baseMatrix, firstOrder, orderStep, pickingId);
		_rootNode->PushRenderCommand(drawContext);
	}

	Matrix4 Canvas::ComputeScaleMatrix() const
	{
		return Matrix4::Scale(Vector2(_scaleFactor, _scaleFactor));
	}

	namespace
	{
		/// @brief node or its closest ancestor having an Interaction, null if none.
		Node* FindInteractive(Node* node)
		{
			while (node != nullptr && node->GetInteraction() == nullptr)
			{
				node = node->GetParent();
			}
			return node;
		}
	}

	/// @brief
	/// @param position
	/// @param pressed
	/// @param scroll
	/// @return
	bool Canvas::UpdatePointer(const Vector2& position, bool pressed, const Vector2& scroll)
	{
		ForgetRemovedNodes();

		Node* hitNode = _rootNode->HitTest(position);

		Node* hoveredNode = FindInteractive(hitNode);
		if (hoveredNode != _hoveredNode.Get())
		{
			Node* previousHoveredNode = _hoveredNode.Get();
			_hoveredNode = hoveredNode;
			SendPointerEvent(previousHoveredNode, PointerEvent::Type::Exit, position);
			SendPointerEvent(_hoveredNode.Get(), PointerEvent::Type::Enter, position); // Null if the Exit's handler destroyed it
		}

		// The handlers may destroy the node hit
		WeakPtr<Node> hit(hitNode);

		if (pressed && _pointerPressed == false)
		{
			_pressHitNode = hitNode;
			_pressPosition = position;
			_lastPosition = position;
			_dragTried = false;
			_dragging = false;
			BubblePointerEvent(hitNode, PointerEvent::Type::PressWithin, position, false);
			ForgetRemovedNodes();
			_pressedNode = BubblePointerEvent(_pressHitNode.Get(), PointerEvent::Type::Press, position, true);
			ForgetRemovedNodes();
			// Following the pointer right away (a Slider): its drag begins with the press
			Node* pressedNode = _pressedNode.Get();
			if (pressedNode != nullptr && pressedNode->GetInteraction() != nullptr && pressedNode->GetInteraction()->UsesDragThreshold() == false)
			{
				TryBeginDrag(position);
			}
		}
		else if (pressed && _pointerPressed)
		{
			if (_dragTried == false && _pressHitNode.Get() != nullptr && Vector2::Distance(position, _pressPosition) > DragThreshold)
			{
				TryBeginDrag(position);
			}
			if (_dragging && _pressedNode.Get() != nullptr && position != _lastPosition)
			{
				PointerEvent event;
				event._type = PointerEvent::Type::Drag;
				event._position = position;
				event._delta = position - _lastPosition;
				SendPointerEvent(_pressedNode.Get(), event);
				ForgetRemovedNodes();
			}
		}
		else if (pressed == false && _pointerPressed && _pressedNode.Get() != nullptr)
		{
			WeakPtr<Node> pressedNode = _pressedNode;
			bool          dragged = _dragging;
			_pressedNode = nullptr;
			_dragging = false;
			if (dragged)
			{
				SendPointerEvent(pressedNode.Get(), PointerEvent::Type::DragEnd, position);
			}
			if (Contains(pressedNode.Get()))
			{
				SendPointerEvent(pressedNode.Get(), PointerEvent::Type::Release, position);
			}
			// The Release handler may have removed or destroyed it, or the node hit
			ForgetRemovedNodes();
			Node* releasedNode = pressedNode.Get();
			Node* hitNow = hit.Get();
			if (dragged == false && Contains(releasedNode) && hitNow != nullptr && (hitNow == releasedNode || hitNow->IsDescendantOf(releasedNode)))
			{
				SendPointerEvent(releasedNode, PointerEvent::Type::Click, position);
			}
		}
		if (pressed == false)
		{
			_pressHitNode = nullptr;
		}
		_pointerPressed = pressed;
		_lastPosition = position;

		if (scroll != Vector2::Zero)
		{
			ForgetRemovedNodes();
			hitNode = _rootNode->HitTest(position); // the events above may have changed the tree
			BubblePointerEvent(hitNode, PointerEvent::Type::Scroll, position, true, scroll);
		}

		return hitNode != nullptr;
	}

	/// @brief
	/// @param position
	void Canvas::TryBeginDrag(const Vector2& position)
	{
		_dragTried = true;
		WeakPtr<Node> dragNode = BubblePointerEvent(_pressHitNode.Get(), PointerEvent::Type::DragBegin, position, true);
		ForgetRemovedNodes();
		if (Contains(dragNode.Get()) == false)
		{
			return;
		}
		// Taken from the node holding the press: it is released, without click (a Button pressed in a ScrollView)
		Node* pressedNode = _pressedNode.Get();
		if (pressedNode != nullptr && pressedNode != dragNode.Get())
		{
			_pressedNode = nullptr;
			SendPointerEvent(pressedNode, PointerEvent::Type::Release, position);
			ForgetRemovedNodes();
			if (Contains(dragNode.Get()) == false)
			{
				return;
			}
		}
		_pressedNode = dragNode;
		_dragging = true;
		_lastPosition = position;
	}

	/// @brief
	void Canvas::ClearPointer()
	{
		ForgetRemovedNodes();

		Node*         hoveredNode = _hoveredNode.Get();
		WeakPtr<Node> pressedNode = _pressedNode;
		bool          dragged = _dragging;
		_hoveredNode = nullptr;
		_pressedNode = nullptr;
		_pressHitNode = nullptr;
		_pointerPressed = false;
		_dragging = false;

		SendPointerEvent(hoveredNode, PointerEvent::Type::Exit, Vector2::Zero);
		if (dragged)
		{
			SendPointerEvent(pressedNode.Get(), PointerEvent::Type::DragEnd, Vector2::Zero);
		}
		if (Contains(pressedNode.Get()))
		{
			SendPointerEvent(pressedNode.Get(), PointerEvent::Type::Release, Vector2::Zero);
		}
	}

	/// @brief
	/// @return
	Node* Canvas::GetHoveredNode() const
	{
		return _hoveredNode.Get();
	}

	/// @brief
	/// @return
	Node* Canvas::GetPressedNode() const
	{
		return _pressedNode.Get();
	}

	/// @brief
	/// @return
	Canvas::ActionEvent& Canvas::GetActionEvent()
	{
		return _actionEvent;
	}

	/// @brief
	/// @param action
	/// @param node
	void Canvas::EmitAction(const String& action, Node& node)
	{
		_actionEvent.Emit(action, node);
	}

	/// @brief
	/// @param node null sends nothing
	/// @param type
	/// @param position
	/// @return whether node's Interaction handled it
	bool Canvas::SendPointerEvent(Node* node, PointerEvent::Type type, const Vector2& position)
	{
		if (node == nullptr || node->GetInteraction() == nullptr)
		{
			return false;
		}

		PointerEvent event;
		event._type = type;
		event._position = position;
		return SendPointerEvent(node, event);
	}

	/// @brief
	/// @param node null sends nothing
	/// @param event its canvas set here
	/// @return whether node's Interaction handled it
	bool Canvas::SendPointerEvent(Node* node, PointerEvent& event)
	{
		if (node == nullptr || node->GetInteraction() == nullptr)
		{
			return false;
		}

		event._canvas = this;
		return node->GetInteraction()->OnPointerEvent(event);
	}

	/// @brief
	/// @param node null gives null
	/// @param type
	/// @param position
	/// @param stopWhenHandled stops at the first Interaction handling it
	/// @param scroll
	/// @return the node that handled it, last one if not stopWhenHandled; null if none. Up the parents of node as they are
	/// after each handler: a node destroyed by its own handler goes on with its parent if it's alive
	Node* Canvas::BubblePointerEvent(Node* node, PointerEvent::Type type, const Vector2& position, bool stopWhenHandled, const Vector2& scroll)
	{
		Node* handledBy = nullptr;
		while (node != nullptr)
		{
			WeakPtr<Node> sent(node);
			WeakPtr<Node> parent(node->GetParent());

			PointerEvent event;
			event._type = type;
			event._position = position;
			event._scroll = scroll;
			if (SendPointerEvent(node, event))
			{
				handledBy = sent.Get();
				if (stopWhenHandled)
				{
					return handledBy;
				}
			}
			node = (sent.Get() != nullptr) ? sent.Get()->GetParent() : parent.Get();
		}
		return handledBy;
	}

	/// @brief
	/// @param node
	/// @return
	bool Canvas::Contains(const Node* node) const
	{
		return node != nullptr && (node == _rootNode || node->IsDescendantOf(_rootNode));
	}

	/// @brief
	void Canvas::ForgetRemovedNodes()
	{
		if (Contains(_hoveredNode.Get()) == false)
		{
			_hoveredNode = nullptr;
		}
		if (Contains(_pressedNode.Get()) == false)
		{
			_pressedNode = nullptr;
			_dragging = false;
		}
		if (Contains(_pressHitNode.Get()) == false)
		{
			_pressHitNode = nullptr;
		}
	}
}
