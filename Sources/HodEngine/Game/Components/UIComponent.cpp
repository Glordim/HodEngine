#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Game/Components/UIComponent.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/UI2/CanvasManager.hpp>

#include <cstdint>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_ENUM(UIComponent::RenderMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(UIComponent::RenderMode::Screen, "Screen");
		reflectionDescriptor.AddEnumValue(UIComponent::RenderMode::World, "World");
	}

	DESCRIBE_REFLECTED_CLASS(UIComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &UIComponent::_prefab, "Prefab", &UIComponent::SetPrefab);
		AddPropertyT(reflectionDescriptor, &UIComponent::_renderMode, "RenderMode", &UIComponent::SetRenderMode);
		AddPropertyT(reflectionDescriptor, &UIComponent::_sortingOrder, "SortingOrder", &UIComponent::SetSortingOrder);
		AddPropertyT(reflectionDescriptor, &UIComponent::_pixelsPerUnit, "PixelsPerUnit", &UIComponent::SetPixelsPerUnit);
		// Named as before the prefab existed: the scenes saved then keep them
		AddPropertyT(reflectionDescriptor, &UIComponent::_overrideScaler, "_overrideScaler", &UIComponent::SetOverrideScaler);
		AddPropertyT(reflectionDescriptor, &UIComponent::_scaler, "_scaler", &UIComponent::SetScaler);
	}

	namespace
	{
		// Above the World canvases (z-orders, 32 bits), the Screen ones by sorting order
		constexpr int64_t ScreenPointerPriority = int64_t(1) << 40;
	}

	UIComponent::UIComponent()
	: _onCanvasAction([this](const String& action, ui2::Node& node) { _actionEvent.Emit(action, node); })
	, _onPrefabSourceChanged(
		  [this](const UID& prefab)
		  {
			  if (_prefab.GetUid().IsValid() && ui2::PrefabInstance::Contains(_prefab.GetUid(), prefab))
			  {
				  BuildCanvas();
			  }
		  })
	{
	}

	UIComponent::~UIComponent()
	{
		DestroyCanvas();
	}

	void UIComponent::OnConstruct()
	{
		_constructed = true;
		ui2::PrefabInstance::GetSourceChangedEvent().Connect(_onPrefabSourceChanged);
		BuildCanvas();
	}

	void UIComponent::OnDestruct()
	{
		_constructed = false;
		_onPrefabSourceChanged.Disconnect();
		DestroyCanvas();
	}

	/// @brief A new canvas, from the prefab as its source is now (see ui2::PrefabInstance::LoadSource: the editor's asset, the
	/// cooked resource in game)
	void UIComponent::BuildCanvas()
	{
		DestroyCanvas();
		if (_constructed == false)
		{
			return;
		}

		_canvas = DefaultAllocator::GetInstance().New<ui2::Canvas>();

		const UID& prefab = _prefab.GetUid();
		if (prefab.IsValid())
		{
			Document document;
			if (ui2::PrefabInstance::LoadSource(prefab, document))
			{
				ui2::PrefabInstance::ScopedLoading loading(prefab);
				_canvas->DeserializeFromDocument(document.GetRootNode());
			}
			else
			{
				OUTPUT_ERROR("UI prefab {} can't be loaded", prefab.ToString());
			}
		}

		_prefabOverrideScaler = _canvas->GetOverrideScaler();
		_prefabScaler = _canvas->GetScaler();
		ApplyScaler();

		_canvas->GetActionEvent().Connect(_onCanvasAction);
		if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
		{
			canvasManager->AddCanvas(_canvas);
		}
		UpdatePointerMapping();
	}

	/// @brief
	void UIComponent::DestroyCanvas()
	{
		if (_canvas == nullptr)
		{
			return;
		}

		_onCanvasAction.Disconnect();
		if (ui2::CanvasManager* canvasManager = ui2::CanvasManager::GetInstance())
		{
			canvasManager->RemoveCanvas(_canvas);
		}
		DefaultAllocator::GetInstance().Delete(_canvas);
		_canvas = nullptr;
	}

	/// @brief The component's scaler on a Screen canvas overriding it, the prefab's otherwise
	void UIComponent::ApplyScaler()
	{
		if (_canvas == nullptr)
		{
			return;
		}

		bool overriding = (_renderMode == RenderMode::Screen && _overrideScaler);
		_canvas->SetScaler(overriding ? _scaler : _prefabScaler);
		_canvas->SetOverrideScaler(overriding ? true : _prefabOverrideScaler);
	}

	/// @brief The priority follows the render mode and the order it is drawn in
	void UIComponent::UpdatePointerMapping()
	{
		if (_canvas == nullptr || ui2::CanvasManager::GetInstance() == nullptr)
		{
			return;
		}

		int64_t priority = ScreenPointerPriority + _sortingOrder;
		if (_renderMode == RenderMode::World)
		{
			Node2dComponent* node2dComponent = (GetOwner() != nullptr) ? GetOwner()->GetComponent<Node2dComponent>() : nullptr;
			priority = (node2dComponent != nullptr) ? node2dComponent->GetZOrder().GetValue() : 0;
		}

		ui2::CanvasManager::GetInstance()->SetPointerMapping(
			_canvas, [this](const Vector2& viewPosition, const Vector2& viewSize, Vector2& outCanvasPosition)
			{ return MapPointer(viewPosition, viewSize, outCanvasPosition); },
			priority);
	}

	/// @brief In game: playing, not paused. The editor's scene view otherwise.
	bool UIComponent::IsPlaying() const
	{
		World* world = GetWorld();
		return world != nullptr && world->GetEditorPlaying() == true && world->GetEditorPaused() == false;
	}

	/// @brief World mode: canvas space to world space
	Matrix4 UIComponent::ComputeWorldCanvasMatrix() const
	{
		float            scale = (_pixelsPerUnit > 0.0f) ? 1.0f / _pixelsPerUnit : 1.0f;
		Node2dComponent* node2dComponent = (GetOwner() != nullptr) ? GetOwner()->GetComponent<Node2dComponent>() : nullptr;
		Matrix4          worldMatrix = (node2dComponent != nullptr) ? node2dComponent->GetWorldMatrix() : Matrix4::Identity;
		return worldMatrix * Matrix4::Scale(Vector2(scale, scale));
	}

	/// @brief The inverse of how PushRenderCommand draws the canvas in game
	bool UIComponent::MapPointer(const Vector2& viewPosition, const Vector2& viewSize, Vector2& outCanvasPosition) const
	{
		if (_canvas == nullptr || IsPlaying() == false || IsEnabledInHierarchy() == false || viewSize.GetX() <= 0.0f || viewSize.GetY() <= 0.0f)
		{
			return false;
		}

		if (_renderMode == RenderMode::Screen)
		{
			// The UI queue: pixels, origin at the view's center, y up
			float scale = _canvas->GetScaleFactor();
			outCanvasPosition = Vector2(viewPosition.GetX() - viewSize.GetX() * 0.5f, viewSize.GetY() * 0.5f - viewPosition.GetY()) / scale;
			return true;
		}

		// Through the camera the world is drawn with (see World::Draw), then the canvas' placement
		CameraComponent* camera = CameraComponent::_main;
		if (camera == nullptr)
		{
			static CameraComponent builtinCamera;
			camera = &builtinCamera;
		}
		camera->SetAspect(viewSize.GetX() / viewSize.GetY());
		Matrix4 view = Matrix4::Identity; // The camera's world matrix (RenderView::SetupCamera inverts it)
		if (Entity* cameraEntity = camera->GetOwner())
		{
			if (NodeComponent* nodeComponent = cameraEntity->GetComponent<NodeComponent>())
			{
				view = nodeComponent->GetWorldMatrix();
			}
		}

		float   ndcX = (2.0f * viewPosition.GetX()) / viewSize.GetX() - 1.0f;
		float   ndcY = 1.0f - (2.0f * viewPosition.GetY()) / viewSize.GetY();
		Vector4 world = view * Matrix4::Inverse(camera->GetProjectionMatrix()) * Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 canvas = Matrix4::Inverse(ComputeWorldCanvasMatrix()) * Vector4(world.GetX() / world.GetW(), world.GetY() / world.GetW(), 0.0f, 1.0f);
		outCanvasPosition = Vector2(canvas.GetX(), canvas.GetY());
		return true;
	}

	void UIComponent::PushRenderCommand(RenderView& renderView)
	{
		if (_canvas == nullptr)
		{
			return;
		}

		// The editor's picking selects the entity from its canvas
		uint32_t pickingId = (GetOwner() != nullptr) ? (uint32_t)GetOwner()->GetInstanceId() : 0;

		if (_renderMode == RenderMode::World)
		{
			Node2dComponent* node2dComponent = (GetOwner() != nullptr) ? GetOwner()->GetComponent<Node2dComponent>() : nullptr;
			uint32_t         order = (node2dComponent != nullptr) ? node2dComponent->GetZOrder().GetValue() : 0;

			_canvas->SetTargetResolution(Vector2::Zero);
			_canvas->UpdateLayout();
			// Sorted as one item at its entity's z-order, its own commands in their order (see ui2::DrawContext)
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::World, ComputeWorldCanvasMatrix(), order, 0, pickingId);
		}
		else if (IsPlaying())
		{
			// The root rect follows the render resolution's ratio (see ui2::CanvasScaler): laid out again right
			// away so a resize never shows one frame of the previous layout.
			_canvas->SetTargetResolution(renderView.GetRenderResolution());
			_canvas->UpdateLayout();
			uint32_t order = (uint32_t)((int64_t)_sortingOrder - INT32_MIN);
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::UI, _canvas->ComputeScaleMatrix(), order, 0, pickingId);
		}
	}

	/// @brief A World canvas' rect in the world. Empty for a Screen canvas, not in the world.
	Rect UIComponent::GetBoundingBox() const
	{
		Rect boundingBox;
		if (_canvas != nullptr && _renderMode == RenderMode::World)
		{
			float scale = (_pixelsPerUnit > 0.0f) ? 1.0f / _pixelsPerUnit : 1.0f;
			boundingBox._size = _canvas->GetLayoutSize() * scale;
		}
		return boundingBox;
	}

	const WeakResource<UIPrefabResource>& UIComponent::GetPrefab() const
	{
		return _prefab;
	}

	// May run before OnConstruct (deserialization): OnConstruct builds the canvas then.
	void UIComponent::SetPrefab(const WeakResource<UIPrefabResource>& prefab)
	{
		_prefab = prefab;
		BuildCanvas();
	}

	UIComponent::RenderMode UIComponent::GetRenderMode() const
	{
		return _renderMode;
	}

	void UIComponent::SetRenderMode(RenderMode renderMode)
	{
		_renderMode = renderMode;
		ApplyScaler();
		UpdatePointerMapping();
	}

	int32_t UIComponent::GetSortingOrder() const
	{
		return _sortingOrder;
	}

	void UIComponent::SetSortingOrder(int32_t sortingOrder)
	{
		_sortingOrder = sortingOrder;
		UpdatePointerMapping();
	}

	float UIComponent::GetPixelsPerUnit() const
	{
		return _pixelsPerUnit;
	}

	void UIComponent::SetPixelsPerUnit(float pixelsPerUnit)
	{
		_pixelsPerUnit = pixelsPerUnit;
	}

	const ui2::CanvasScaler& UIComponent::GetScaler() const
	{
		return _scaler;
	}

	// May run before OnConstruct (deserialization): the canvas built then gets it.
	void UIComponent::SetScaler(const ui2::CanvasScaler& scaler)
	{
		_scaler = scaler;
		ApplyScaler();
	}

	bool UIComponent::GetOverrideScaler() const
	{
		return _overrideScaler;
	}

	// May run before OnConstruct (deserialization), see SetScaler.
	void UIComponent::SetOverrideScaler(bool overrideScaler)
	{
		_overrideScaler = overrideScaler;
		ApplyScaler();
	}

	ui2::Canvas* UIComponent::GetCanvas() const
	{
		return _canvas;
	}

	ui2::Node* UIComponent::GetRootNode() const
	{
		if (_canvas == nullptr || _canvas->GetRootNode()->GetChildren().Empty())
		{
			return nullptr;
		}
		return _canvas->GetRootNode()->GetChildren()[0];
	}

	ui2::Node* UIComponent::FindNode(std::string_view path) const
	{
		ui2::Node* rootNode = GetRootNode();
		return (rootNode != nullptr) ? rootNode->FindByPath(path) : nullptr;
	}

	UIComponent::ActionEvent& UIComponent::GetActionEvent()
	{
		return _actionEvent;
	}
}
