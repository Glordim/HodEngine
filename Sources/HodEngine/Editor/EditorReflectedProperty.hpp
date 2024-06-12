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

        EditorReflectedProperty(void* instance, void* sourceInstance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent);
        EditorReflectedProperty(const std::vector<void*>& instances, void* sourceInstance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent);
        ~EditorReflectedProperty();

    public:

        bool IsEditingMultipleInstance() const;
        bool IsOverride() const;

        ReflectionProperty* GetReflectionProperty() const;

        void*                       GetSourceInstance() const;
        void*                       GetInstance() const;
        const std::vector<void*>&   GetInstances() const;

        EditorReflectedObject*   GetParent();
        EditorReflectedObject*   GetEditorReflectedObject();

    private:

        void*                   _sourceInstance = nullptr; 
        std::vector<void*>      _instances;
        ReflectionProperty*     _reflectionProperty = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;

        EditorReflectedObject*  _parent = nullptr;
        EditorReflectedObject*  _reflectedObject = nullptr;
    }; 
}