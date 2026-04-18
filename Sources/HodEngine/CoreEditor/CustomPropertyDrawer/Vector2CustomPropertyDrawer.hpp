#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	/// @brief
	class HOD_CORE_EDITOR_API Vector2CustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		Vector2CustomPropertyDrawer() = default;
		Vector2CustomPropertyDrawer(const Vector2CustomPropertyDrawer&) = delete;
		Vector2CustomPropertyDrawer(Vector2CustomPropertyDrawer&&) = delete;
		~Vector2CustomPropertyDrawer() override = default;

		Vector2CustomPropertyDrawer& operator=(const Vector2CustomPropertyDrawer&) = delete;
		Vector2CustomPropertyDrawer& operator=(Vector2CustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
