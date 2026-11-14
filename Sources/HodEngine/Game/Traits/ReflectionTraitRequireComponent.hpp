#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionTrait.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	/// @brief On a component type: the entity must also have a component of the given type (or deriving from it).
	/// Entity::AddComponent adds it first when it is missing, Entity::RemoveComponent refuses to remove it while it is required.
	/// Several per type, and inherited: a type requires what it declares and what its parents declare (see GetRequiredComponents).
	/// The required type must be instantiable (it is added as is). Not checked when loading: the data stays as written.
	/// Given by its GetReflectionDescriptor function, read when needed: two types may require each other (their descriptors are built
	/// lazily, calling the other's GetReflectionDescriptor while describing one would recurse forever).
	/// HOD_TRAIT(ReflectionTraitRequireComponent, &Node2dComponent::GetReflectionDescriptor);
	class HOD_GAME_API ReflectionTraitRequireComponent : public ReflectionTrait
	{
		RTTI(ReflectionTraitRequireComponent, ReflectionTrait)

	public:
		using DescriptorGetter = ReflectionDescriptor& (*)();

		ReflectionTraitRequireComponent(DescriptorGetter requiredComponent);
		ReflectionTraitRequireComponent(const ReflectionTraitRequireComponent& copy) = default;
		ReflectionTraitRequireComponent(ReflectionTraitRequireComponent&& move) = default;
		~ReflectionTraitRequireComponent() override = default;

		ReflectionTraitRequireComponent& operator=(const ReflectionTraitRequireComponent& copy) = default;
		ReflectionTraitRequireComponent& operator=(ReflectionTraitRequireComponent&& move) = default;

	public:
		const ReflectionDescriptor& GetRequiredComponent() const;

		/// @brief Every component type required by this one: its own traits, then its parents' (no duplicates)
		static Vector<const ReflectionDescriptor*> GetRequiredComponents(const ReflectionDescriptor& componentDescriptor);

	private:
		DescriptorGetter _requiredComponent = nullptr;
	};
}
