#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	/// @brief A short sentence telling what a type (or a property) is for, shown by the editor (tooltips)
	class HOD_CORE_API ReflectionTraitDescription : public ReflectionTrait
	{
		RTTI(ReflectionTraitDescription, ReflectionTrait)

	public:
		ReflectionTraitDescription(const std::string_view& description);
		ReflectionTraitDescription(const ReflectionTraitDescription& copy) = default;
		ReflectionTraitDescription(ReflectionTraitDescription&& move) = default;
		~ReflectionTraitDescription() = default;

		ReflectionTraitDescription& operator=(const ReflectionTraitDescription& copy) = default;
		ReflectionTraitDescription& operator=(ReflectionTraitDescription&& move) = default;

	public:
		const String& GetValue() const;

	private:
		String _description;
	};
}
