#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/Rtti.hpp"

namespace hod::inline core
{
	///@brief
	class HOD_CORE_API ReflectionTrait
	{
		RTTI_BASE(ReflectionTrait)

	public:
		ReflectionTrait() = default;
		ReflectionTrait(const ReflectionTrait& copy) = default;
		ReflectionTrait(ReflectionTrait&& move) = default;
		virtual ~ReflectionTrait() = default;

		ReflectionTrait& operator=(const ReflectionTrait& copy) = default;
		ReflectionTrait& operator=(ReflectionTrait&& move) = default;
	};
}
