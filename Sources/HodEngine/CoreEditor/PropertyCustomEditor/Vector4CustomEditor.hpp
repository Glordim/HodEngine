#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
	class Vector4;
}

namespace hod::editor
{
	/// @brief
	class HOD_CORE_EDITOR_API Vector4CustomEditor : public CustomPropertyDrawer
	{
	public:
		Vector4CustomEditor() = default;
		Vector4CustomEditor(const Vector4CustomEditor&) = delete;
		Vector4CustomEditor(Vector4CustomEditor&&) = delete;
		~Vector4CustomEditor() override = default;

		Vector4CustomEditor& operator=(const Vector4CustomEditor&) = delete;
		Vector4CustomEditor& operator=(Vector4CustomEditor&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
