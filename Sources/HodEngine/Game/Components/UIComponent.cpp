#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Game/Components/UIComponent.hpp"
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/CanvasManager.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

#include <HodEngine/UI2/Node.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(UIComponent, reflectionDescriptor)
	{
		//AddPropertyT(reflectionDescriptor, &UIComponent::_canvas, "Canvas");
		AddPropertyT(reflectionDescriptor, &UIComponent::_scaler, "_scaler", &UIComponent::SetScaler);
	}

	UIComponent::UIComponent()
	{

	}

	UIComponent::~UIComponent()
	{

	}

	void UIComponent::OnConstruct()
	{
		_canvas = DefaultAllocator::GetInstance().New<ui2::Canvas>();
		_canvas->SetScaler(_scaler);

		_canvas->AddChild(DefaultAllocator::GetInstance().New<ui2::Node>());

		CanvasManager::GetInstance()->AddCanvas(_canvas);
	}

	void UIComponent::OnDestruct()
	{
		CanvasManager::GetInstance()->RemoveCanvas(_canvas);
		DefaultAllocator::GetInstance().Delete(_canvas);
		_canvas = nullptr;
	}

	void UIComponent::PushRenderCommand(RenderView& renderView)
	{
		if (_canvas == nullptr)
		{
			return;
		}

		World* world = GetWorld();
		if (world != nullptr && world->GetEditorPlaying() == true && world->GetEditorPaused() == false)
		{
			// The root rect follows the render resolution's ratio (see ui2::CanvasScaler): laid out again right
			// away so a resize never shows one frame of the previous layout.
			_canvas->SetTargetResolution(renderView.GetRenderResolution());
			_canvas->UpdateLayout();
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::UI, _canvas->ComputeScaleMatrix());
		}
		else
		{
			_canvas->SetTargetResolution(Vector2::Zero);
			_canvas->UpdateLayout();
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::World, Matrix4::Scale(Vector2(0.01f, 0.01f)));
		}
	}

	Rect UIComponent::GetBoundingBox() const
	{
		return Rect();
	}

	const ui2::CanvasScaler& UIComponent::GetScaler() const
	{
		return _scaler;
	}

	// May run before OnConstruct (deserialization): OnConstruct then applies _scaler to the new canvas.
	void UIComponent::SetScaler(const ui2::CanvasScaler& scaler)
	{
		_scaler = scaler;
		if (_canvas != nullptr)
		{
			_canvas->SetScaler(_scaler);
		}
	}
}
