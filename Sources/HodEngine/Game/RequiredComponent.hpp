#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <cassert>
#include <type_traits>

/// @brief In a DESCRIBE_REFLECTED_CLASS: the RequiredComponent member MEMBER, the requirement of the type and its access
#define HOD_REQUIRED_COMPONENT(MEMBER) hod::AddRequiredComponent(reflectionDescriptor, &ReflectedClassType::MEMBER)

namespace hod::inline game
{
	template<typename _Component_>
	class RequiredComponent;

	/// @brief What fills a RequiredComponent (see AddRequiredComponent)
	struct RequiredComponentAccess
	{
		template<typename _Component_>
		static void Set(RequiredComponent<_Component_>& member, _Component_* component)
		{
			member._component = component;
		}
	};

	/// @brief A member of a component: the component of its entity it requires, and its access. Declared with
	/// HOD_REQUIRED_COMPONENT(_node2d): the requirement of the type (see ReflectionTraitRequireComponent: added with it, at loading
	/// too, can't be removed while it is there), then filled before its OnConstruct. Never null afterwards, for as long as the
	/// component lives. Not a property: not serialized, not shown. Its entity's only: a component of a parent or of a child may
	/// change or go at any time (GetComponentInParent when needed)
	template<typename _Component_>
	class RequiredComponent
	{
		friend struct RequiredComponentAccess;

	public:
		_Component_* Get() const
		{
			assert(_component != nullptr && "RequiredComponent not filled: declared with HOD_REQUIRED_COMPONENT? used before OnConstruct?");
			return _component;
		}

		_Component_* operator->() const
		{
			return Get();
		}

		_Component_& operator*() const
		{
			return *Get();
		}

		operator _Component_*() const
		{
			return Get();
		}

	private:
		_Component_* _component = nullptr;
	};

	/// @brief HOD_REQUIRED_COMPONENT: the requirement, filling the member (see ReflectionTraitRequireComponent::Assign)
	template<typename _Class_, typename _Component_>
	void AddRequiredComponent(ReflectionDescriptor& descriptor, RequiredComponent<_Component_> _Class_::* member)
	{
		static_assert(std::is_base_of<Component, _Class_>::value, "A RequiredComponent is a member of a component");
		static_assert(std::is_base_of<Component, _Component_>::value, "A RequiredComponent requires a component");
		descriptor.AddTrait<ReflectionTraitRequireComponent>(&_Component_::GetReflectionDescriptor,
		                                                     [member](Component& requiring, Component* required)
		                                                     {
			                                                     RequiredComponentAccess::Set(static_cast<_Class_&>(requiring).*member, static_cast<_Component_*>(required));
		                                                     });
	}

	/// @brief HOD_PROPERTY of a RequiredComponent: refused, it isn't a property
	template<typename _Class_, typename _Component_>
	ReflectionProperty* AddPropertyT(ReflectionDescriptor&, RequiredComponent<_Component_> _Class_::*, const char*)
	{
		static_assert(sizeof(_Class_*) == 0, "A RequiredComponent isn't a property: HOD_REQUIRED_COMPONENT(member)");
		return nullptr;
	}
}
