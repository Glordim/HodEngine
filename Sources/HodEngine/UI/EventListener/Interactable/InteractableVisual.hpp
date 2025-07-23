#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Drawables/Drawable.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/WeakComponent.hpp>
#include <HodEngine/Core/Resource/WeakResource.hpp>

namespace hod::ui
{
	class HOD_UI_API InteractableVisual : public game::Component
	{
		REFLECTED_CLASS(InteractableVisual, game::Component)

	public:

		void	Refresh();

	private:

		game::WeakComponent<Drawable>	_target;

		Color	_normalColor;
		Color	_highlightedColor;
		Color	_pressedColor;
		Color	_disabledColor;
		Color	_disabledHighlightedColor;
	};
}
