#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}

namespace hod::inline game
{
	class ZOrder;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API ZOrderCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:

								ZOrderCustomPropertyDrawer() = default;
								ZOrderCustomPropertyDrawer(const ZOrderCustomPropertyDrawer&) = delete;
								ZOrderCustomPropertyDrawer(ZOrderCustomPropertyDrawer&&) = delete;
								~ZOrderCustomPropertyDrawer() override = default;

		ZOrderCustomPropertyDrawer&		operator = (const ZOrderCustomPropertyDrawer&) = delete;
		ZOrderCustomPropertyDrawer&		operator = (ZOrderCustomPropertyDrawer&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool				Draw(game::ZOrder& value);
	};
}
