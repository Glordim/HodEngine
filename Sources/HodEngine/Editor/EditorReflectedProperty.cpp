#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>

namespace hod::editor
{
    EditorReflectedProperty::EditorReflectedProperty(void* instance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent)
    : _instances({ instance })
    , _reflectionProperty(reflectionProperty)
    , _parent(parent)
    {

    }

    EditorReflectedProperty::EditorReflectedProperty(const std::vector<void*>& instances, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent)
    : _instances(instances)
    , _reflectionProperty(reflectionProperty)
    , _parent(parent)
    {

    }

    EditorReflectedProperty::~EditorReflectedProperty()
    {
        delete _reflectedObject;
    }

    ReflectionProperty* EditorReflectedProperty::GetReflectionProperty() const
    {
        return _reflectionProperty;
    }

    EditorReflectedObject* EditorReflectedProperty::GetParent()
    {
        return _parent;
    }

    void* EditorReflectedProperty::GetInstance() const
    {
        return _instances[0];
    }

    const std::vector<void*>& EditorReflectedProperty::GetInstances() const
    {
        return _instances;
    }

    EditorReflectedObject* EditorReflectedProperty::GetEditorReflectedObject()
    {
        if (_reflectedObject == nullptr && _reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
        {
            _reflectedObject = new EditorReflectedObject(*this);
        }
        return _reflectedObject;
    }
}