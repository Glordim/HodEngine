#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

#undef min
#undef max

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Canvas, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Canvas::_scaleMode, "ScaleMode", &Canvas::SetScaleMode);
		AddPropertyT(reflectionDescriptor, &Canvas::_widthHeightPreferredAxis, "WidthHeightPreferredAxis", &Canvas::SetWidthHeightPreferredAxis);
	}

	void Canvas::OnEnable()
	{
		_rootNode = GetOwner()->GetComponent<Node>();
		_renderModeMatrix = Matrix4::Scale(Vector2(0.01f, 0.01f));
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
			RecomputeRootNodeSize();
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
			RecomputeRootNodeSize();
		}
	}

	void Canvas::RecomputeRootNodeSize()
	{
		Vector2 resolution(1920.0f, 1080.0f);
		Vector2 designSize(1920.0f, 1080.0f);

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
	}

	Rect Canvas::GetBoundingBox() const
	{
		return Rect();
	}

	void Canvas::PushRenderCommand(renderer::RenderView& renderView)
	{
		if (_renderMode == RenderMode::Camera)
		{
			RecomputeRootNodeSize(renderView.GetRenderResolution());

			game::World* world = GetOwner()->GetScene()->GetWorld();
			if (world->GetEditorPlaying() == true && world->GetEditorPaused() == false)
			{
				_renderModeMatrix = renderView.GetViewMatrix() * Matrix4::Scale(Vector2(0.01f, 0.01f));
			}
		}

		static std::function<void(Node*, renderer::RenderView&)> drawRecursively = [&](Node* node, renderer::RenderView& renderView)
		{
			if (node == nullptr || node->GetOwner()->IsActiveInHierarchy() == false)
			{
				return;
			}

			Drawable* drawable = node->GetOwner()->GetComponent<Drawable>();
			if (drawable != nullptr)
			{
				drawable->PushRenderCommand(renderView);
			}

			for (const game::WeakEntity& child : node->GetOwner()->GetChildren())
			{
				Node* childNode = child.Lock()->GetComponent<Node>();
				drawRecursively(childNode, renderView);
			}
		};
		drawRecursively(_rootNode.Get(), renderView);
	}

	const Matrix4& Canvas::GetRenderModeMatrix() const
	{
		return _renderModeMatrix;
	}
}
