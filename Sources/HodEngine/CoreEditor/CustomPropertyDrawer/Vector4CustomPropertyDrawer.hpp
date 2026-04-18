#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}

namespace hod::inline editor
{
	/// @brief
	class HOD_CORE_EDITOR_API Vector4CustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		Vector4CustomPropertyDrawer() = default;
		Vector4CustomPropertyDrawer(const Vector4CustomPropertyDrawer&) = delete;
		Vector4CustomPropertyDrawer(Vector4CustomPropertyDrawer&&) = delete;
		~Vector4CustomPropertyDrawer() override = default;

		Vector4CustomPropertyDrawer& operator=(const Vector4CustomPropertyDrawer&) = delete;
		Vector4CustomPropertyDrawer& operator=(Vector4CustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}
