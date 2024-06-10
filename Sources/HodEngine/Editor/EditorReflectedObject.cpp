#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/PrefabUtility.hpp>

namespace hod::editor
{
    EditorReflectedObject::EditorReflectedObject(std::shared_ptr<game::Component> component)
    : _instances({ component.get() })
    , _reflectionDescriptor(component->GetReflectionDescriptorV())
    {
        std::shared_ptr<game::Component> sourceComponent = game::PrefabUtility::GetCorrespondingComponent(component);

        GeneratePropertiesFromReflectionDescriptor();
    }

    EditorReflectedObject::EditorReflectedObject(const std::vector<std::shared_ptr<game::Component>>& components)
    {
        // todo;
    }

    EditorReflectedObject::EditorReflectedObject(void* instance, ReflectionDescriptor* reflectionDescriptor)
    : _instances({ instance })
    , _reflectionDescriptor(reflectionDescriptor)
    {
        GeneratePropertiesFromReflectionDescriptor();
    }

    EditorReflectedObject::EditorReflectedObject(const std::vector<void*>& instances, ReflectionDescriptor* reflectionDescriptor)
    : _instances(instances)
    , _reflectionDescriptor(reflectionDescriptor)
    {
        GeneratePropertiesFromReflectionDescriptor();
    }

    EditorReflectedObject::EditorReflectedObject(EditorReflectedProperty& sourceProperty)
    : _sourceProperty(&sourceProperty)
    {
        ReflectionPropertyObject* reflectionPropertyObject = static_cast<ReflectionPropertyObject*>(sourceProperty.GetReflectionProperty());
        _reflectionDescriptor = reflectionPropertyObject->GetReflectionDescriptor();
        _instances.reserve(sourceProperty.GetInstances().size());
        for (void* instance : sourceProperty.GetInstances())
        {
            _instances.push_back(reflectionPropertyObject->GetInstance(instance));
        }

        GeneratePropertiesFromReflectionDescriptor();
    }

    EditorReflectedObject::~EditorReflectedObject()
    {
        for (auto* property : _properties)
        {
            delete property; // avoid alloc ?
        }
    }

    void* EditorReflectedObject::GetInstance() const
    {
        return _instances[0];
    }

    const std::vector<void*>& EditorReflectedObject::GetInstances() const
    {
        return _instances;
    }

    EditorReflectedProperty* EditorReflectedObject::GetSourceProperty() const
    {
        return _sourceProperty;
    }

    void EditorReflectedObject::GeneratePropertiesFromReflectionDescriptor()
    {
        for (ReflectionProperty* reflectionProperty : _reflectionDescriptor->GetProperties())
        {
            EditorReflectedProperty* property = new EditorReflectedProperty(_instances, reflectionProperty, this);
            _properties.push_back(property);
        }
    }

    std::vector<EditorReflectedProperty*>& EditorReflectedObject::GetProperties()
    {
        return _properties;
    }
}
