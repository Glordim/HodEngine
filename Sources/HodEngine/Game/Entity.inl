#pragma once

#include <cstdint>
#include <string>

namespace hod::game
{
	template<typename _Component_>
	std::weak_ptr<_Component_> Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");

		for (const std::shared_ptr<Component>& component : _components)
		{
			if (component->GetMetaType() == _Component_::GetMetaTypeStatic())
			{
				return std::static_pointer_cast<_Component_>(component);
			}
		}

		return std::weak_ptr<_Component_>();
	}

	template<typename _Component_>
	std::weak_ptr<_Component_> Entity::AddComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use AddComponent()");

		for (const std::shared_ptr<Component>& component : _components)
		{
			if (component->GetMetaType() == _Component_::GetMetaTypeStatic())
			{
				return std::static_pointer_cast<_Component_>(component);
			}
		}

		std::shared_ptr<_Component_> component = std::make_shared<_Component_>(weak_from_this());
		_components.push_back(component);

		return component;
	}
}