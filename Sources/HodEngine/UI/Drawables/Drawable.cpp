#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"
#include "HodEngine/UI/Node.hpp"

#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Drawable, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Drawable::_color, "Color", &Drawable::SetColor);
	}

	void Drawable::OnEnable()
	{
		game::Entity* owner = GetOwner();
		if (owner != nullptr)
		{
			_node = owner->GetComponent<Node>();
		}
	}

	void Drawable::SetColor(const Color& color)
	{
		_color = color;
	}

	const Color& Drawable::GetColor() const
	{
		return _color;
	}
}
