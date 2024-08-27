#include "HodEngine/Game/Pch.hpp"
#include "WeakResource.hpp"

#include "HodEngine/Core/Resource.hpp"
#include "HodEngine/Core/ResourceManager.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp>

namespace hod::game
{
    DESCRIBE_REFLECTED_CLASS_NO_PARENT(WeakResourceBase)
    {
        ADD_PROPERTY(WeakResourceBase, _uid);
    }

    /// @brief 
    /// @param pointer 
    WeakResourceBase::WeakResourceBase(ReflectionDescriptor* resourceDescriptor)
    : _resourceDescriptor(resourceDescriptor)
    , _uid(UID::INVALID_UID)
    {
        
    }

    /// @brief 
    /// @param pointer 
    WeakResourceBase::WeakResourceBase(ReflectionDescriptor* resourceDescriptor, const std::shared_ptr<Resource>& pointer)
    : _resourceDescriptor(resourceDescriptor)
    , _uid(UID::INVALID_UID)
    , _pointer(pointer)
    {
    }

    /// @brief 
    WeakResourceBase::~WeakResourceBase()
    {
    }

    /// @brief 
    /// @param pointer 
    WeakResourceBase& WeakResourceBase::operator = (const WeakResourceBase& copy)
    {
        _uid = copy._uid;
        _pointer = copy._pointer;
        _resourceDescriptor = copy._resourceDescriptor;
        return *this;
    }

    /// @brief 
    /// @param pointer 
    WeakResourceBase& WeakResourceBase::operator = (const std::weak_ptr<Resource>& pointer)
    {
        _pointer = pointer.lock();
        return *this;
    }

    /// @brief 
    /// @param other 
    /// @return 
    bool WeakResourceBase::operator==(const WeakResourceBase& other) const
    {
        return _uid == other._uid;
    }

    /// @brief 
    /// @return 
    std::shared_ptr<Resource> WeakResourceBase::Lock() const
    {
        if (_uid == UID::INVALID_UID)
        {
            _pointer = nullptr;
            return _pointer;
        }
        else if (_pointer == nullptr || _pointer->GetUid() != _uid)
        {
            _pointer = ResourceManager::GetInstance()->GetResource(_resourceDescriptor, _uid);
        }
        return _pointer;
    }

    /// @brief 
    /// @return 
    const UID& WeakResourceBase::GetUid() const
    {
        return _uid;
    }

    void WeakResourceBase::SetUid(const UID& uid)
    {
        _uid = uid;
    }

    ReflectionDescriptor* WeakResourceBase::GetResourceDescriptor() const
    {
        return _resourceDescriptor;
    }
}
