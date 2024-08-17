#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	class EditorReflectedObject;

	///@brief 
	class HOD_EDITOR_API CustomPropertyDrawer
	{
	public:

									CustomPropertyDrawer() = default;
									CustomPropertyDrawer(const CustomPropertyDrawer&) = delete;
									CustomPropertyDrawer(CustomPropertyDrawer&&) = delete;
			virtual					~CustomPropertyDrawer() = default;

			CustomPropertyDrawer&	operator = (const CustomPropertyDrawer&) = delete;
			CustomPropertyDrawer&	operator = (CustomPropertyDrawer&&) = delete;

	public:

		virtual bool				Draw(EditorReflectedObject& reflectedObject) = 0;
	};
}
