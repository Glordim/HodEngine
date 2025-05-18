#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::game
{
	class ZOrder;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API ZOrderCustomEditor : public CustomPropertyDrawer
	{
	public:

								ZOrderCustomEditor() = default;
								ZOrderCustomEditor(const ZOrderCustomEditor&) = delete;
								ZOrderCustomEditor(ZOrderCustomEditor&&) = delete;
								~ZOrderCustomEditor() override = default;

		ZOrderCustomEditor&		operator = (const ZOrderCustomEditor&) = delete;
		ZOrderCustomEditor&		operator = (ZOrderCustomEditor&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool				Draw(game::ZOrder& value);
	};
}
