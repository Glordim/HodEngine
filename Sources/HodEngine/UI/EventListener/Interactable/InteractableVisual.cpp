#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/EventListener/Interactable/InteractableVisual.hpp"

#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(InteractableVisual, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &InteractableVisual::_target, "_target");

		AddPropertyT(reflectionDescriptor, &InteractableVisual::_normalColor, "_normalColor");
		AddPropertyT(reflectionDescriptor, &InteractableVisual::_highlightedColor, "_highlightedColor");
		AddPropertyT(reflectionDescriptor, &InteractableVisual::_pressedColor, "_pressedColor");
		AddPropertyT(reflectionDescriptor, &InteractableVisual::_disabledColor, "_disabledColor");
		AddPropertyT(reflectionDescriptor, &InteractableVisual::_disabledHighlightedColor, "_disabledHighlightedColor");
	}

	void InteractableVisual::Refresh()
	{
		Drawable* drawable = _target.Lock();
		if (drawable == nullptr)
		{
			drawable = GetOwner()->GetComponent<Drawable>();
		}
		if (drawable == nullptr)
		{
			return;
		}

		drawable->SetColor(_normalColor); // TODO
	}
}
