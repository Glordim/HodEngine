#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"

#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>

namespace hod::inline editor
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
        if (_reflectedObject == nullptr && (_reflectionProperty->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic() || (_reflectionProperty->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic() && static_cast<ReflectionPropertyArray*>(_reflectionProperty)->GetElementReflectionDescriptor() != nullptr)))
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

        if (_reflectionProperty->GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
        {
            return (static_cast<ReflectionPropertyVariable*>(_reflectionProperty)->CompareInstance(_instances[0], _sourceInstance) == false);
        }
        else if (_reflectionProperty->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
        {
            return _reflectedObject->IsOverride();
        }
        return false;
    }

    bool EditorReflectedProperty::IsEditingMultipleInstance() const
    {
        return _instances.Size() > 1;
    }

    namespace
    {
        bool AreObjectsEqual(const ReflectionDescriptor& descriptor, const void* left, const void* right);

        /// @brief Whether the property has the same value in both owners
        bool AreValuesEqual(const ReflectionProperty& property, const void* left, const void* right)
        {
            if (property.GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
            {
                return static_cast<const ReflectionPropertyVariable&>(property).CompareInstance(left, right);
            }
            else if (property.GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
            {
                const ReflectionPropertyObject& objectProperty = static_cast<const ReflectionPropertyObject&>(property);
                return AreObjectsEqual(*objectProperty.GetReflectionDescriptor(), objectProperty.GetValue(left), objectProperty.GetValue(right));
            }
            else if (property.GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic())
            {
                // The elements aren't compared: the arrays are only edited one instance at a time (see PropertyDrawer::DrawPropertyArray)
                const ReflectionPropertyArray& arrayProperty = static_cast<const ReflectionPropertyArray&>(property);
                return arrayProperty.GetElementCount(left) == arrayProperty.GetElementCount(right);
            }
            return true;
        }

        /// @brief With the type's operator== when it has one, property by property otherwise
        bool AreObjectsEqual(const ReflectionDescriptor& descriptor, const void* left, const void* right)
        {
            if (descriptor.CanCompare())
            {
                return descriptor.Compare(left, right);
            }
            for (const ReflectionDescriptor* current = &descriptor; current != nullptr; current = current->GetParent())
            {
                for (const ReflectionProperty* property : current->GetProperties())
                {
                    if (AreValuesEqual(*property, left, right) == false)
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    bool EditorReflectedProperty::HasDifferentValues() const
    {
        for (uint32_t index = 1; index < _instances.Size(); ++index)
        {
            bool equal = true;
            if (_internalIndex != (uint32_t)-1)
            {
                // An array element: compared as an object
                const ReflectionPropertyArray* arrayProperty = static_cast<const ReflectionPropertyArray*>(_reflectionProperty);
                if (arrayProperty->GetElementReflectionDescriptor() != nullptr)
                {
                    equal = AreObjectsEqual(*arrayProperty->GetElementReflectionDescriptor(), GetObjectOf<void>(0), GetObjectOf<void>(index));
                }
            }
            else
            {
                equal = AreValuesEqual(*_reflectionProperty, _instances[0], _instances[index]);
            }

            if (equal == false)
            {
                return true;
            }
        }
        return false;
    }

    bool EditorReflectedProperty::IsArray() const
    {
        return (_reflectionProperty->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic());
    }

    uint32_t EditorReflectedProperty::GetArraySize() const
    {
        if (_reflectionProperty->GetRttiType() == ReflectionPropertyArray::GetRttiTypeStatic())
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
