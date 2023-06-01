#pragma once

namespace hod::core
{
    class ReflectionProperty;
}

namespace hod::editor
{
    class PropertyDrawer
    {
    public:

                        PropertyDrawer() = delete;
                        PropertyDrawer(const PropertyDrawer&) = delete;
                        PropertyDrawer(PropertyDrawer&&) = delete;
                        ~PropertyDrawer() = delete;

        PropertyDrawer& operator = (const PropertyDrawer&) = delete;
        PropertyDrawer& operator = (PropertyDrawer&&) = delete;

    public:

        static void     DrawProperty(core::ReflectionProperty* property);
        // static void  RegisterCustomDrawer(); //TODO
    }
}
