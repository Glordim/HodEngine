#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod
{
	///@brief
	class HOD_CORE_API ReflectionTraitHide : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitHide, ReflectionTrait)

	public:
		ReflectionTraitHide() = default;
		ReflectionTraitHide(const ReflectionTraitHide& copy) = default;
		ReflectionTraitHide(ReflectionTraitHide&& move) = default;
		~ReflectionTraitHide() override = default;

		ReflectionTraitHide& operator=(const ReflectionTraitHide& copy) = default;
		ReflectionTraitHide& operator=(ReflectionTraitHide&& move) = default;
	};
}
