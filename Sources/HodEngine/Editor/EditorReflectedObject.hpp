#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <vector>
#include <memory>

namespace hod
{
    class ReflectionDescriptor;
}

namespace hod::game
{
    class Component;
}

namespace hod::editor
{
    class EditorReflectedProperty;
    
    class HOD_EDITOR_API EditorReflectedObject
    {
    public:

        EditorReflectedObject(std::shared_ptr<game::Component> component);
        EditorReflectedObject(const std::vector<std::shared_ptr<game::Component>>& components);

        EditorReflectedObject(void* instance, ReflectionDescriptor* reflectionDescriptor);
        EditorReflectedObject(const std::vector<void*>& instances, ReflectionDescriptor* reflectionDescriptor);
        
        EditorReflectedObject(EditorReflectedProperty& sourceProperty);
        ~EditorReflectedObject();

    public:

        bool IsEditingMultipleInstance() const;
        bool IsOverride() const;

        std::vector<EditorReflectedProperty*>&  GetProperties();

        void*                       GetInstance() const;
        const std::vector<void*>&   GetInstances() const;

        EditorReflectedProperty*    GetSourceProperty() const;

    private:

        void GeneratePropertiesFromReflectionDescriptor(ReflectionDescriptor* reflectionDescriptor);

    private:

        void*                   _sourceInstance = nullptr;
        std::vector<void*>      _instances;
        ReflectionDescriptor*   _reflectionDescriptor = nullptr;
        EditorReflectedProperty*    _sourceProperty = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;
    }; 
}