#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	/// @brief Icon editor tooling shows next to a reflected type's name (hierarchy, library, type pickers...):
	/// a UTF-8 glyph of the editor's icon font, e.g. one of the ICON_MDI_* macros. Inherited by subtypes
	/// that declare none (FindTrait falls back on the parent).
	class HOD_CORE_API ReflectionTraitIcon : public ReflectionTrait
	{
		RTTI(ReflectionTraitIcon, ReflectionTrait)

	public:
		ReflectionTraitIcon(const std::string_view& icon);
		ReflectionTraitIcon(const ReflectionTraitIcon& copy) = default;
		ReflectionTraitIcon(ReflectionTraitIcon&& move) = default;
		~ReflectionTraitIcon() override = default;

		ReflectionTraitIcon& operator=(const ReflectionTraitIcon& copy) = default;
		ReflectionTraitIcon& operator=(ReflectionTraitIcon&& move) = default;

	public:
		const String& GetValue() const;

	private:
		String _icon;
	};
}
