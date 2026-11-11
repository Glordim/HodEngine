#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	/// @brief Where a type is shown in the editor's lists (Add Component...): a folder path, its levels separated by '/'
	/// ("Physics/Colliders"). Inherited by the derived types that don't declare their own.
	class HOD_CORE_API ReflectionTraitCategory : public ReflectionTrait
	{
		RTTI(ReflectionTraitCategory, ReflectionTrait)

	public:
		ReflectionTraitCategory(const std::string_view& category);
		ReflectionTraitCategory(const ReflectionTraitCategory& copy) = default;
		ReflectionTraitCategory(ReflectionTraitCategory&& move) = default;
		~ReflectionTraitCategory() = default;

		ReflectionTraitCategory& operator=(const ReflectionTraitCategory& copy) = default;
		ReflectionTraitCategory& operator=(ReflectionTraitCategory&& move) = default;

	public:
		/// @brief Without '/' at its ends
		const String& GetValue() const;

	private:
		String _category;
	};
}
