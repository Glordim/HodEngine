#pragma once

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	class EditorReflectedObject;

	///@brief 
	class CustomPropertyDrawer
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
