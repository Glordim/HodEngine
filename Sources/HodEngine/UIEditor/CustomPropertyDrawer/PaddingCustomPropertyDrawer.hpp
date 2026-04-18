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
	class HOD_UI_EDITOR_API PaddingCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:

								PaddingCustomPropertyDrawer() = default;
								PaddingCustomPropertyDrawer(const PaddingCustomPropertyDrawer&) = delete;
								PaddingCustomPropertyDrawer(PaddingCustomPropertyDrawer&&) = delete;
								~PaddingCustomPropertyDrawer() override = default;

		PaddingCustomPropertyDrawer&	operator = (const PaddingCustomPropertyDrawer&) = delete;
		PaddingCustomPropertyDrawer&	operator = (PaddingCustomPropertyDrawer&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
