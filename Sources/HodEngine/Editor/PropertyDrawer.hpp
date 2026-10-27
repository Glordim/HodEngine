#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline core
{
    class ReflectionDescriptor;
	class ReflectionProperty;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;
}

namespace hod::inline editor
{
    class EditorReflectedObject;
    class EditorReflectedProperty;

    class HOD_EDITOR_API PropertyDrawer
    {
    public:

                        PropertyDrawer() = delete;
                        PropertyDrawer(const PropertyDrawer&) = delete;
                        PropertyDrawer(PropertyDrawer&&) = delete;
                        ~PropertyDrawer() = delete;

        PropertyDrawer& operator = (const PropertyDrawer&) = delete;
        PropertyDrawer& operator = (PropertyDrawer&&) = delete;

    public:

        static bool     DrawDescriptor(EditorReflectedObject& reflectedObject);
        static bool     DrawProperty(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyVariable(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyArray(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyObject(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);
        static bool     DrawContextMenu(EditorReflectedProperty& editorReflectedProperty);

		/// @brief Left out of the resource pickers drawn until the pop (the font itself, the ones already in a list...). Not copied: alive until then
		/// TODO: belongs to WeakResourceCustomPropertyDrawer (CoreEditor), the one that reads it. Here only because the font editors, its users, are still in Editor,
		/// which CoreEditor depends on: to move there with them once they go to a RendererEditor module
		static void     PushExcludedResources(const Vector<UID>& excludedResources);
		static void     PopExcludedResources();
		static bool     IsResourceExcluded(const UID& uid);

	private:

		static Vector<const Vector<UID>*> _excludedResources;
    };
}
