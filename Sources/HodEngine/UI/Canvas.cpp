#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/Rebuildable.hpp"

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Renderer/RenderView.hpp>

#undef min
#undef max

namespace hod::ui
{
	DESCRIBE_REFLECTED_ENUM(Canvas::RenderMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Canvas::RenderMode::Camera, "Camera");
		reflectionDescriptor.AddEnumValue(Canvas::RenderMode::World, "World");
	}

	DESCRIBE_REFLECTED_ENUM(Canvas::ScaleMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Canvas::ScaleMode::WidthHeight, "WidthHeight");
		reflectionDescriptor.AddEnumValue(Canvas::ScaleMode::Expand, "Expand");
		reflectionDescriptor.AddEnumValue(Canvas::ScaleMode::Shrink, "Shrink");
		reflectionDescriptor.AddEnumValue(Canvas::ScaleMode::Fixed, "Fixed");
	}

	DESCRIBE_REFLECTED_CLASS(Canvas, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Canvas::_renderMode, "RenderMode", &Canvas::SetRenderMode);
		AddPropertyT(reflectionDescriptor, &Canvas::_scaleMode, "ScaleMode", &Canvas::SetScaleMode);
		AddPropertyT(reflectionDescriptor, &Canvas::_widthHeightPreferredAxis, "WidthHeightPreferredAxis", &Canvas::SetWidthHeightPreferredAxis);
	}

	void Canvas::OnEnable()
	{
		_rootNode = GetOwner()->GetComponent<Node>();
	}

	void Canvas::SetRenderMode(RenderMode renderMode)
	{
		_renderMode = renderMode;
	}

	Canvas::RenderMode Canvas::GetRenderMode() const
	{
		return _renderMode;
	}

	void Canvas::SetCamera(game::CameraComponent* camera)
	{
		_camera = camera;
	}

	game::CameraComponent* Canvas::GetCamera() const
	{
		return _camera.Get();
	}

	Canvas::ScaleMode Canvas::GetScaleMode() const
	{
		return _scaleMode;
	}

	void Canvas::SetScaleMode(ScaleMode scaleMode)
	{
		if (_scaleMode != scaleMode)
		{
			_scaleMode = scaleMode;
			// TODO dirty flag to recompute _scaleFactor + _rootNode size ?
		}
	}

	float Canvas::GetWidthHeightPreferredAxis() const
	{
		return _widthHeightPreferredAxis;
	}

	void Canvas::SetWidthHeightPreferredAxis(float widthHeightPreferredAxis)
	{
		if (_widthHeightPreferredAxis != widthHeightPreferredAxis)
		{
			_widthHeightPreferredAxis = widthHeightPreferredAxis;
			// TODO dirty flag to recompute _scaleFactor + _rootNode size ?
		}
	}

	void Canvas::RecomputeRootNodeSize(const Vector2& resolution)
	{
		static Vector2 designSize(1920.0f, 1080.0f);

		switch (_scaleMode)
		{
			case ScaleMode::WidthHeight:
			{
				float logWidth = std::log2f(resolution.GetX() / designSize.GetX());
				float logHeight = std::log2f(resolution.GetY() / designSize.GetY());
				float logLerp = std::lerp(logWidth, logHeight, _widthHeightPreferredAxis);
				_scaleFactor = std::powf(2.0f, logLerp);
			}
			break;

			case ScaleMode::Expand:
			{
				_scaleFactor = std::min(resolution.GetX() / designSize.GetX(), resolution.GetY() / designSize.GetY());
			}
			break;

			case ScaleMode::Shrink:
			{
				_scaleFactor = std::max(resolution.GetX() / designSize.GetX(), resolution.GetY() / designSize.GetY());
			}
			break;

			case ScaleMode::Fixed:
			{
				_scaleFactor = 1.0f;
			}
			break;
		}

		Vector2 rootNodeSize = designSize;
		if (_scaleMode != ScaleMode::Fixed)
		{
			rootNodeSize.SetX(resolution.GetX() / _scaleFactor);
			rootNodeSize.SetY(resolution.GetY() / _scaleFactor);
		}

		game::World* world = GetOwner()->GetScene()->GetWorld();
		if (world->GetEditorPlaying() == true && world->GetEditorPaused() == false)
		{
			_rootNode->SetDeltaSize(rootNodeSize);
		}
	}

	Rect Canvas::GetBoundingBox() const
	{
		return Rect();
	}

	void Canvas::PushRenderCommand(renderer::RenderView& renderView)
	{
		renderer::RenderView::RenderQueueType renderQueueType = renderer::RenderView::RenderQueueType::UI;
		if (_renderMode == RenderMode::Camera)
		{
			RecomputeRootNodeSize(renderView.GetRenderResolution());

			game::World* world = GetOwner()->GetScene()->GetWorld();
			if (world->GetEditorPlaying() == true && world->GetEditorPaused() == false)
			{
				_renderModeMatrix = Matrix4::Scale(Vector2::One * _scaleFactor);
				renderQueueType = renderer::RenderView::RenderQueueType::UI;
			}
			else
			{
				_scaleFactor = 1.0f;
				_renderModeMatrix = Matrix4::Scale(Vector2(0.01f, 0.01f) * _scaleFactor);
				renderQueueType = renderer::RenderView::RenderQueueType::World;
			}
		}
		else
		{
			_scaleFactor = 1.0f;
			_renderModeMatrix = Matrix4::Scale(Vector2(0.01f, 0.01f) * _scaleFactor);
			renderQueueType = renderer::RenderView::RenderQueueType::World;
		}

		int32_t                                                                                                   zOrder = 0;
		static std::function<void(Node*, renderer::RenderView&, renderer::RenderView::RenderQueueType, int32_t&)> drawRecursively =
			[&](Node* node, renderer::RenderView& renderView, renderer::RenderView::RenderQueueType renderQueueType, int32_t& zOrder)
		{
			if (node == nullptr || node->GetOwner()->IsActiveInHierarchy() == false)
			{
				return;
			}

			node->SetZOrder(zOrder);
			++zOrder;

			Drawable* drawable = node->GetOwner()->GetComponent<Drawable>();
			if (drawable != nullptr)
			{
				drawable->PushRenderCommand(renderView, renderQueueType);
			}

			for (const game::WeakEntity& child : node->GetOwner()->GetChildren())
			{
				Node* childNode = child.Lock()->GetComponent<Node>();
				drawRecursively(childNode, renderView, renderQueueType, zOrder);
			}
		};
		drawRecursively(_rootNode.Get(), renderView, renderQueueType, zOrder);
	}

	const Matrix4& Canvas::GetRenderModeMatrix() const
	{
		return _renderModeMatrix;
	}

	void Canvas::OnUpdate(float /*deltaTime*/)
	{
		DoRebuild();
	}

	void Canvas::DoRebuild()
	{
		while (_markedForRebuild.empty() == false)
		{
			Rebuildable* rebuildable = _markedForRebuild[0].Get();
			if (rebuildable != nullptr)
			{
				rebuildable->Rebuild();
			}
			_markedForRebuild.erase(_markedForRebuild.begin());
		}
	}

	void Canvas::MarkForRebuild(Rebuildable* rebuildable)
	{
		for (const WeakPtr<Rebuildable>& existingRebuildable : _markedForRebuild)
		{
			if (existingRebuildable.Get() == rebuildable)
			{
				return;
			}
		}
		_markedForRebuild.emplace_back(rebuildable);
	}
}
