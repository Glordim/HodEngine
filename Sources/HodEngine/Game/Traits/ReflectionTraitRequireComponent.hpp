#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionTrait.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <functional>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	class Component;

	/// @brief On a component type: the entity must also have a component of the given type (or deriving from it).
	/// Entity::AddComponent adds it first when it is missing, Entity::RemoveComponent refuses to remove it while it is required.
	/// Several per type, and inherited: a type requires what it declares and what its parents declare (see GetRequiredComponents).
	/// The required type must be instantiable (it is added as is). Added when loading too, after the components as written (a requirement
	/// declared since the data was saved), saved with them next time.
	/// Declared by a RequiredComponent<T> member (HOD_REQUIRED_COMPONENT): then the trait also fills it when the component is constructed (see Assign).
	/// Given by its GetReflectionDescriptor function, read when needed: two types may require each other (their descriptors are built
	/// lazily, calling the other's GetReflectionDescriptor while describing one would recurse forever).
	/// HOD_TRAIT(ReflectionTraitRequireComponent, &Node2dComponent::GetReflectionDescriptor);
	class HOD_GAME_API ReflectionTraitRequireComponent : public ReflectionTrait
	{
		RTTI(ReflectionTraitRequireComponent, ReflectionTrait)

	public:
		using DescriptorGetter = ReflectionDescriptor& (*)();

		/// @brief Fills the member that caches it (see RequiredComponent): the requiring component, the required one (null if missing)
		using AssignFunction = std::function<void(Component& requiring, Component* required)>;

		ReflectionTraitRequireComponent(DescriptorGetter requiredComponent, AssignFunction assign = nullptr);
		ReflectionTraitRequireComponent(const ReflectionTraitRequireComponent& copy) = default;
		ReflectionTraitRequireComponent(ReflectionTraitRequireComponent&& move) = default;
		~ReflectionTraitRequireComponent() override = default;

		ReflectionTraitRequireComponent& operator=(const ReflectionTraitRequireComponent& copy) = default;
		ReflectionTraitRequireComponent& operator=(ReflectionTraitRequireComponent&& move) = default;

	public:
		const ReflectionDescriptor& GetRequiredComponent() const;

		/// @brief Fills the member caching it, if the requirement was declared by one (see RequiredComponent)
		/// @return false if the entity doesn't have it (an error is logged)
		bool Assign(Component& requiring) const;

		/// @brief Every RequiredComponent member of the component (of its type and its parents'): filled before its OnConstruct
		static void AssignRequiredComponents(Component& component);

		/// @brief Every component type required by this one: its own traits, then its parents' (no duplicates)
		static Vector<const ReflectionDescriptor*> GetRequiredComponents(const ReflectionDescriptor& componentDescriptor);

	private:
		DescriptorGetter _requiredComponent = nullptr;
		AssignFunction   _assign;
	};
}
