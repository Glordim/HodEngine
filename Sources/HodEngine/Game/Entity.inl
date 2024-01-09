#pragma once

#include <cstdint>
#include <string>

#include "HodEngine/Game/Component.hpp"

namespace hod::game
{
	template<typename _Component_>
	std::weak_ptr<_Component_> Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");

		for (const std::shared_ptr<Component>& component : _components)
		{
			if (component->HasCompatibleMetaType(_Component_::GetMetaTypeStatic()) == true)
			{
				return std::static_pointer_cast<_Component_>(component);
			}
		}

		return std::weak_ptr<_Component_>();
	}

	template<typename _Component_>
	std::weak_ptr<_Component_> Entity::AddComponent(bool awakeAndStart)
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use AddComponent()");

		std::shared_ptr<_Component_> existingComponent = GetComponent<_Component_>().lock();
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		std::shared_ptr newComponent = std::make_shared<_Component_>();
		AddComponent(newComponent, awakeAndStart);
		return std::static_pointer_cast<_Component_>(newComponent);
	}

	template<typename _Component_>
	void Entity::RemoveComponent()
	{
		auto it = std::find(_components.begin(), _components.end(), [](std::shared_ptr<Component> component)
		{
			return component->HasCompatibleMetaType(_Component_::GetMetaTypeStatic());
		});
		if (it != _components.end())
		{
			_components.erase(it);
		}
	}
}