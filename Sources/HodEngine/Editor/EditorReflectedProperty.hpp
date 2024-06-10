#pragma once

#include <vector>

namespace hod
{
    class ReflectionProperty;
}

namespace hod::editor
{
    class EditorReflectedObject;

    class EditorReflectedProperty
    {
    public:

        EditorReflectedProperty(void* instance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent);
        EditorReflectedProperty(const std::vector<void*>& instances, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent);
        ~EditorReflectedProperty();

    public:

        bool IsEditingMultipleInstance() const;

        ReflectionProperty* GetReflectionProperty() const;

        void*                       GetInstance() const;
        const std::vector<void*>&   GetInstances() const;

        EditorReflectedObject*   GetParent();
        EditorReflectedObject*   GetEditorReflectedObject();

    private:

        std::vector<void*>      _instances;
        ReflectionProperty*     _reflectionProperty = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;

        EditorReflectedObject*  _parent = nullptr;
        EditorReflectedObject*  _reflectedObject = nullptr;
    }; 
}