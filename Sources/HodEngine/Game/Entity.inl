#pragma once

#include <cstdint>
#include <string>

#include "HodEngine/Game/Component.hpp"

namespace hod::game
{
	template<typename _Component_>
	std::shared_ptr<_Component_> Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");
		return std::static_pointer_cast<_Component_>(GetComponent(_Component_::GetMetaTypeStatic()));
	}

	template<typename _Component_>
	std::shared_ptr<_Component_> Entity::GetComponentInParent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");
		return std::static_pointer_cast<_Component_>(GetComponentInParent(_Component_::GetMetaTypeStatic()));
	}

	template<typename _Component_>
	std::shared_ptr<_Component_> Entity::AddComponent(bool awakeAndStart)
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use AddComponent()");
		return std::static_pointer_cast<_Component_>(AddComponent(_Component_::GetMetaTypeStatic(), awakeAndStart));
	}

	template<typename _Component_>
	void Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use RemoveComponent()");
		RemoveComponent(_Component_::GetMetaTypeStatic());
	}
}