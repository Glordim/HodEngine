#pragma once

#include <vector>

namespace hod
{
    class ReflectionDescriptor;
}

namespace hod::editor
{
    class EditorReflectedProperty;
    
    class EditorReflectedObject
    {
    public:

        EditorReflectedObject(void* instance, ReflectionDescriptor* reflectionDescriptor);
        EditorReflectedObject(const std::vector<void*>& instances, ReflectionDescriptor* reflectionDescriptor);

    public:

        bool IsEditingMultipleInstance() const;

    private:

        std::vector<void*>      _instances;
        ReflectionDescriptor*   _reflectionDescriptor = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;
    }; 
}