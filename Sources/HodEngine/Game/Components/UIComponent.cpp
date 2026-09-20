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
		(void)reflectionDescriptor;
		//AddPropertyT(reflectionDescriptor, &UIComponent::_canvas, "Canvas");
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
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::UI, _canvas->ComputeFitMatrix(renderView.GetRenderResolution()));
		}
		else
		{
			_canvas->PushRenderCommand(renderView, RenderView::RenderQueueType::World, Matrix4::Scale(Vector2(0.01f, 0.01f)));
		}
	}

	Rect UIComponent::GetBoundingBox() const
	{
		return Rect();
	}
}
