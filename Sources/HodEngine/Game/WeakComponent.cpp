#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitSerializedByCallback.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::game
{
    DESCRIBE_REFLECTED_CLASS(WeakComponentBase, reflectionDescriptor)
    {
        reflectionDescriptor.AddTrait<ReflectionTraitSerializedByCallback>();
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor)
    : _componentDescriptor(componentDescriptor)
    {
        
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor, Component* pointer)
    : _componentDescriptor(componentDescriptor)
    , _pointer(pointer)
    {
    }

    /// @brief 
    WeakComponentBase::~WeakComponentBase()
    {
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase& WeakComponentBase::operator = (const WeakComponentBase& copy)
    {
        _pointer = copy._pointer;
        _componentDescriptor = copy._componentDescriptor;
        return *this;
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase& WeakComponentBase::operator = (Component* pointer)
    {
        _pointer = pointer;
        return *this;
    }

    /// @brief 
    /// @param other 
    /// @return 
    bool WeakComponentBase::operator==(const WeakComponentBase& other) const
    {
        return _pointer.Get() == other._pointer.Get();
    }

    /// @brief 
    /// @return 
    Component* WeakComponentBase::Lock() const
    {
        return _pointer.Get();
    }

    /// @brief 
    /// @param pointer 
    void WeakComponentBase::SetPointer(Component* pointer)
    {
        _pointer = pointer;
    }

    /// @brief 
    /// @return 
    ReflectionDescriptor* WeakComponentBase::GetComponentDescriptor() const
    {
        return _componentDescriptor;
    }
}
