#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Components/RendererComponent.hpp>
#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>

namespace hod::ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Drawable : public game::RendererComponent
	{
		REFLECTED_CLASS(Drawable, game::RendererComponent)

	public:

		void			OnEnable() override;

		void			SetColor(const Color& color);
		const Color&	GetColor() const;

		Rect			GetBoundingBox() const override;

	protected:

	 	Color				_color;

		WeakPtr<Node>		_node;
	};
}
