#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <functional>

namespace hod::inline core
{
	///@brief
	class HOD_CORE_API ReflectionTraitNoSerialization : public ReflectionTrait
	{
		RTTI(ReflectionTraitNoSerialization, ReflectionTrait)

	public:
		ReflectionTraitNoSerialization() = default;
		ReflectionTraitNoSerialization(const ReflectionTraitNoSerialization& copy) = default;
		ReflectionTraitNoSerialization(ReflectionTraitNoSerialization&& move) = default;
		~ReflectionTraitNoSerialization() = default;

		ReflectionTraitNoSerialization& operator=(const ReflectionTraitNoSerialization& copy) = default;
		ReflectionTraitNoSerialization& operator=(ReflectionTraitNoSerialization&& move) = default;
	};
}
