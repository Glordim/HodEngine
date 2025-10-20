#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod
{
	///@brief
	class HOD_CORE_API ReflectionTraitDisplayName : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitDisplayName, ReflectionTrait)

	public:
		ReflectionTraitDisplayName(const std::string_view& name);
		ReflectionTraitDisplayName(const ReflectionTraitDisplayName& copy) = default;
		ReflectionTraitDisplayName(ReflectionTraitDisplayName&& move) = default;
		~ReflectionTraitDisplayName() = default;

		ReflectionTraitDisplayName& operator=(const ReflectionTraitDisplayName& copy) = default;
		ReflectionTraitDisplayName& operator=(ReflectionTraitDisplayName&& move) = default;

	public:
		const String& GetValue() const;

	private:
		String _name;
	};
}
