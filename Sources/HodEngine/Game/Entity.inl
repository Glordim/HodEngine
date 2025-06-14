#pragma once

#include <cstdint>
#include "HodEngine/Core/String.hpp"

#include "HodEngine/Game/Component.hpp"

namespace hod::game
{
	template<typename _Component_>
	_Component_* Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");
		return static_cast<_Component_*>(GetComponent(_Component_::GetReflectionDescriptor()));
	}

	template<typename _Component_>
	_Component_* Entity::GetComponentInParent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");
		return static_cast<_Component_*>(GetComponentInParent(_Component_::GetReflectionDescriptor()));
	}

	template<typename _Component_>
	_Component_* Entity::AddComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use AddComponent()");
		return static_cast<_Component_*>(AddComponent(_Component_::GetReflectionDescriptor()));
	}

	template<typename _Component_>
	void Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use RemoveComponent()");
		RemoveComponent(_Component_::GetReflectionDescriptor());
	}
}
