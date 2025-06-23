#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Components/RendererComponent.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>

namespace hod::game
{
	class Entity;
}

namespace hod::ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Canvas : public game::RendererComponent
	{
		REFLECTED_CLASS(Canvas, game::RendererComponent)

	public:

		enum class ScaleMode
		{
			WidthHeight,
			Expand,
			Shrink,
			Fixed,
		};

	public:

		void				OnEnable() override;

		ScaleMode			GetScaleMode() const;
		void				SetScaleMode(ScaleMode scaleMode);

		float				GetWidthHeightPreferredAxis() const;
		void				SetWidthHeightPreferredAxis(float widthHeightPreferredAxis);
		Rect				GetBoundingBox() const override;
		void				PushRenderCommand(renderer::RenderView& renderView) override;

		const Matrix4&		GetRenderModeMatrix() const;

	private:

		WeakPtr<Node>		_rootNode;

		ScaleMode			_scaleMode = ScaleMode::WidthHeight;
		float				_widthHeightPreferredAxis = 1.0f;

		float				_scaleFactor = 1.0f;

		Matrix4				_renderModeMatrix;
	};
}
