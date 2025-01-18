#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <vector>

#undef GetObject

namespace hod
{
    class ReflectionProperty;
}

namespace hod::editor
{
    class EditorReflectedObject;

    class HOD_EDITOR_API EditorReflectedProperty
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

        bool                        HasDifferentValues() const;

        template<typename _type_>
        _type_                      GetValue() const;

        template<typename _type_>
        _type_*                     GetObject() const;

        template<typename _type_>
        void                        SetValue(_type_ value) const;

        template<typename _type_>
        void                        SetObject(const _type_& value) const;

        EditorReflectedObject*   GetParent();
        EditorReflectedObject*   GetEditorReflectedObject();

    private:

        std::vector<void*>      _instances;
        void*                   _sourceInstance = nullptr; 
        ReflectionProperty*     _reflectionProperty = nullptr;

        std::vector<EditorReflectedProperty*>    _properties;

        EditorReflectedObject*  _parent = nullptr;
        EditorReflectedObject*  _reflectedObject = nullptr;
    };

    template<typename _type_>
    _type_ EditorReflectedProperty::GetValue() const
    {
        if (_reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
        {
            return static_cast<ReflectionPropertyVariable*>(_reflectionProperty)->GetValue<_type_>(_instances[0]);
        }
        else
        {
            assert(false);
            return _type_();
        }
    }

    template<typename _type_>
    _type_* EditorReflectedProperty::GetObject() const
    {
        if (_reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
        {
            return static_cast<_type_*>(static_cast<ReflectionPropertyObject*>(_reflectionProperty)->GetValue(_instances[0]));
        }
        else
        {
            assert(false);
            return nullptr;
        }
    }

    template<typename _type_>
    void EditorReflectedProperty::SetValue(_type_ value) const
    {
        if (_reflectionProperty->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
        {
            for (void* instance : _instances)
            {
                static_cast<ReflectionPropertyVariable*>(_reflectionProperty)->SetValue<_type_>(instance, value);
            }
        }
    }

    template<typename _type_>
    void EditorReflectedProperty::SetObject(const _type_& value) const
    {
        if (_reflectionProperty->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
        {
            for (void* instance : _instances)
            {
                static_cast<ReflectionPropertyObject*>(_reflectionProperty)->SetValue(instance, (const void*)&value);
            }
        }
    }
}
