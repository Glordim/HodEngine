#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::inline ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Drawable : public Component
	{
		REFLECTED_CLASS(Drawable, Component)

	public:

		void			OnEnable() override;

		void			SetColor(const Color& color);
		const Color&	GetColor() const;

		virtual void	PushRenderCommand(renderer::RenderView& renderView, renderer::RenderView::RenderQueueType renderQueueType) = 0;

	protected:

	 	Color				_color;

		WeakPtr<Node>		_node;
	};
}
