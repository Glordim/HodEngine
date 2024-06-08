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

        EditorReflectedProperty(void* instance, ReflectionProperty* reflectionProperty);
        EditorReflectedProperty(const std::vector<void*>& instances, ReflectionProperty* reflectionProperty);

    public:

        bool IsEditingMultipleInstance() const;

    private:

        std::vector<void*>      _instances;
        ReflectionProperty*     _reflectionDescriptor = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;
    }; 
}