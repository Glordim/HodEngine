#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

namespace hod::renderer
{
	class RenderQueue;
}

namespace hod::ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Drawable : public game::Component
	{
		REFLECTED_CLASS(Drawable, game::Component)

	public:

		void			OnEnable() override;

		void			SetColor(const Color& color);
		const Color&	GetColor() const;

		virtual void	PushRenderCommand(renderer::RenderView& renderView) = 0;

	protected:

	 	Color				_color;

		WeakPtr<Node>		_node;
	};
}
