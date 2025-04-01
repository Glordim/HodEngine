#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
	class Vector2;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API Vector2CustomEditor : public CustomPropertyDrawer
	{
	public:

								Vector2CustomEditor() = default;
								Vector2CustomEditor(const Vector2CustomEditor&) = delete;
								Vector2CustomEditor(Vector2CustomEditor&&) = delete;
								~Vector2CustomEditor() override = default;

		Vector2CustomEditor&	operator = (const Vector2CustomEditor&) = delete;
		Vector2CustomEditor&	operator = (Vector2CustomEditor&&) = delete;

	public:

		bool					Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool				Draw(Vector2& value);
	};
}
