#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Canvas.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/DrawContext.hpp"
#include "HodEngine/UI2/UISettings.hpp"

#include <HodEngine/Core/Document/DocumentNode.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Math/Rect.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(CanvasScaler::Mode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Fit, "Fit");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Expand, "Expand");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Shrink, "Shrink");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::MatchWidth, "MatchWidth");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::MatchHeight, "MatchHeight");
	}

	DESCRIBE_REFLECTED_CLASS(CanvasScaler, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CanvasScaler::_designResolution, "DesignResolution");
		AddPropertyT(reflectionDescriptor, &CanvasScaler::_mode, "Mode");
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

	/// @brief
	/// @param deltaTime
	void Canvas::Update(float deltaTime)
	{
		UpdateInteractions(_rootNode, deltaTime);
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
		/// @brief Whether node is root or one of its descendants. Compares addresses only: node may be a deleted one.
		bool ContainsNode(const Node* root, const Node* node)
		{
			if (root == node)
			{
				return true;
			}
			for (const Node* child : root->GetChildren())
			{
				if (ContainsNode(child, node))
				{
					return true;
				}
			}
			return false;
		}

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
	/// @return
	bool Canvas::UpdatePointer(const Vector2& position, bool pressed)
	{
		ForgetRemovedNodes();

		Node* hitNode = _rootNode->HitTest(position);

		Node* hoveredNode = FindInteractive(hitNode);
		if (hoveredNode != _hoveredNode)
		{
			Node* previousHoveredNode = _hoveredNode;
			_hoveredNode = hoveredNode;
			SendPointerEvent(previousHoveredNode, PointerEvent::Type::Exit, position);
			SendPointerEvent(_hoveredNode, PointerEvent::Type::Enter, position);
		}

		if (pressed && _pointerPressed == false)
		{
			for (Node* node = hitNode; node != nullptr; node = node->GetParent())
			{
				if (SendPointerEvent(node, PointerEvent::Type::Press, position))
				{
					_pressedNode = node;
					break;
				}
			}
		}
		else if (pressed == false && _pointerPressed && _pressedNode != nullptr)
		{
			Node* pressedNode = _pressedNode;
			_pressedNode = nullptr;
			SendPointerEvent(pressedNode, PointerEvent::Type::Release, position);
			// The Release handler may have removed it
			ForgetRemovedNodes();
			if (ContainsNode(_rootNode, pressedNode) && hitNode != nullptr && (hitNode == pressedNode || hitNode->IsDescendantOf(pressedNode)))
			{
				SendPointerEvent(pressedNode, PointerEvent::Type::Click, position);
			}
		}
		_pointerPressed = pressed;

		return hitNode != nullptr;
	}

	/// @brief
	void Canvas::ClearPointer()
	{
		ForgetRemovedNodes();

		Node* hoveredNode = _hoveredNode;
		Node* pressedNode = _pressedNode;
		_hoveredNode = nullptr;
		_pressedNode = nullptr;
		_pointerPressed = false;

		SendPointerEvent(hoveredNode, PointerEvent::Type::Exit, Vector2::Zero);
		SendPointerEvent(pressedNode, PointerEvent::Type::Release, Vector2::Zero);
	}

	/// @brief
	/// @return
	Node* Canvas::GetHoveredNode() const
	{
		return _hoveredNode;
	}

	/// @brief
	/// @return
	Node* Canvas::GetPressedNode() const
	{
		return _pressedNode;
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
		event._canvas = this;
		return node->GetInteraction()->OnPointerEvent(event);
	}

	/// @brief
	void Canvas::ForgetRemovedNodes()
	{
		if (_hoveredNode != nullptr && ContainsNode(_rootNode, _hoveredNode) == false)
		{
			_hoveredNode = nullptr;
		}
		if (_pressedNode != nullptr && ContainsNode(_rootNode, _pressedNode) == false)
		{
			_pressedNode = nullptr;
		}
	}
}
