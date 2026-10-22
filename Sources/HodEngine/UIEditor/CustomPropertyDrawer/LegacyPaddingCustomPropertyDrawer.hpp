#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_UI_EDITOR_API LegacyPaddingCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:

								LegacyPaddingCustomPropertyDrawer() = default;
								LegacyPaddingCustomPropertyDrawer(const LegacyPaddingCustomPropertyDrawer&) = delete;
								LegacyPaddingCustomPropertyDrawer(LegacyPaddingCustomPropertyDrawer&&) = delete;
								~LegacyPaddingCustomPropertyDrawer() override = default;

		LegacyPaddingCustomPropertyDrawer&	operator = (const LegacyPaddingCustomPropertyDrawer&) = delete;
		LegacyPaddingCustomPropertyDrawer&	operator = (LegacyPaddingCustomPropertyDrawer&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue = false) override;
	};
}
