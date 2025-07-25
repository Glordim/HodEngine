#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>

namespace hod::editor
{
    EditorReflectedProperty::EditorReflectedProperty(void* instance, void* sourceInstance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent)
    : _instances({ instance })
    , _sourceInstance(sourceInstance)
    , _reflectionProperty(reflectionProperty)
    , _parent(parent)
    {

    }

    EditorReflectedProperty::EditorReflectedProperty(const Vector<void*>& instances, void* sourceInstance, ReflectionProperty* reflectionProperty, EditorReflectedObject* parent)
    : _instances(instances)
    , _sourceInstance(sourceInstance)
    , _reflectionProperty(reflectionProperty)
    , _parent(parent)
    {

    }

    EditorReflectedProperty::~EditorReflectedProperty()
    {
        DefaultAllocator::GetInstance().Delete(_reflectedObject);
    }

    ReflectionProperty* EditorReflectedProperty::GetReflectionProperty() const
    {
        return _reflectionProperty;
    }

    EditorReflectedObject* EditorReflectedProperty::GetParent()
    {
        return _parent;
    }

    void* EditorReflectedProperty::GetSourceInstance() const
    {
        return _sourceInstance;
    }

    void* EditorReflectedProperty::GetInstance() const
    {
        return _instances[0];
    }

    const Vector<void*>& EditorReflectedProperty::GetInstances() const
    {
        return _instances;
    }

    EditorReflectedObject* EditorReflectedProperty::GetEditorReflectedObject()
    {
        if (_reflectedObject == nullptr && (_reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic() || (_reflectionProperty->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic() && static_cast<ReflectionPropertyArray*>(_reflectionProperty)->GetElementReflectionDescriptor() != nullptr)))
        {
            _reflectedObject = DefaultAllocator::GetInstance().New<EditorReflectedObject>(*this);
        }
        return _reflectedObject;
    }

    bool EditorReflectedProperty::IsOverride() const
    {
        if (_sourceInstance == nullptr)
        {
            return false;
        }

        if (_reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
        {
            return (static_cast<ReflectionPropertyVariable*>(_reflectionProperty)->CompareInstance(_instances[0], _sourceInstance) == false);
        }
        else if (_reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
        {
            return _reflectedObject->IsOverride();
        }
        return false;
    }

    bool EditorReflectedProperty::IsArray() const
    {
        return (_reflectionProperty->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic());
    }

    uint32_t EditorReflectedProperty::GetArraySize() const
    {
        if (_reflectionProperty->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic())
        {
            return static_cast<ReflectionPropertyArray*>(_reflectionProperty)->GetElementCount(_instances[0]);
        }
        else
        {
            assert(false);
            return 0;
        }
    }

    EditorReflectedProperty EditorReflectedProperty::GenerateElementProperty(uint32_t index) const
    {
        EditorReflectedProperty elementProperty(_instances[0], nullptr, _reflectionProperty, _reflectedObject); // todo support multiselection
        elementProperty._internalIndex = index;
        return elementProperty;
    }

    uint32_t EditorReflectedProperty::GetInternalIndex() const
    {
        return _internalIndex;
    }
}
