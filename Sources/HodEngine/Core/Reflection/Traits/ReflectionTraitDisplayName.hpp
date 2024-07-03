#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <string>

namespace hod
{
	///@brief 
	class HOD_API ReflectionTraitDisplayName : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitDisplayName, ReflectionTrait)

	public:

										ReflectionTraitDisplayName(const std::string_view& name);
										ReflectionTraitDisplayName(const ReflectionTraitDisplayName& copy) = default;
										ReflectionTraitDisplayName(ReflectionTraitDisplayName&& move) = default;
										~ReflectionTraitDisplayName() = default;

		ReflectionTraitDisplayName&		operator = (const ReflectionTraitDisplayName& copy) = default;
		ReflectionTraitDisplayName&		operator = (ReflectionTraitDisplayName&& move) = default;

	public:

		const std::string&				GetValue() const;

	private:

		std::string						_name;
	};
}
