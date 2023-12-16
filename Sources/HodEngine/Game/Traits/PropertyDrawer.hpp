#pragma once

#if defined(HOD_EDITOR)

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::game
{
	///@brief 
	class PropertyDrawer
	{
	public:

									PropertyDrawer() = default;
									PropertyDrawer(const PropertyDrawer&) = delete;
									PropertyDrawer(PropertyDrawer&&) = delete;
			virtual					~PropertyDrawer() = default;

			PropertyDrawer&			operator = (const PropertyDrawer&) = delete;
			PropertyDrawer&			operator = (PropertyDrawer&&) = delete;

	public:

		virtual bool				Draw(void* instance, ReflectionPropertyObject* property) = 0;
	};
}

#endif
