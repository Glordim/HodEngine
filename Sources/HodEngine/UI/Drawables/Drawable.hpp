#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Drawable : public game::Component
	{
		REFLECTED_CLASS(Drawable, game::Component)

	public:

		void			OnEnable() override;

		void			SetColor(const math::Color& color);
		const math::Color&	GetColor() const;

		virtual void	PushRenderCommand(renderer::RenderView& renderView, renderer::RenderView::RenderQueueType renderQueueType) = 0;

	protected:

	 	math::Color				_color;

		WeakPtr<Node>		_node;
	};
}
