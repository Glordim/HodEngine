#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::game
{
    std::map<UID, std::weak_ptr<Component>> WeakComponentMapping::_map;

    void WeakComponentMapping::Insert(const UID& uid, std::weak_ptr<Component> component)
    {
        _map[uid] = component;
    }

    void WeakComponentMapping::Clear()
    {
        _map.clear();
    };

    std::shared_ptr<Component> WeakComponentMapping::Resolve(const UID& uid)
    {
        auto it = _map.find(uid);
        if (it == _map.end())
        {
            return std::shared_ptr<Component>();
        }
        else
        {
            return it->second.lock();
        }
    }

    DESCRIBE_REFLECTED_CLASS_NO_PARENT(WeakComponentBase)
    {
        AddTrait<ReflectionTraitCustomSerialization>(
            [](const void* instance, Document::Node& documentNode)
            {
                const WeakComponentBase* weakComponentBase = static_cast<const WeakComponentBase*>(instance);
                UID uid = weakComponentBase->GetForSerialization();
                Serializer::Serialize(uid, documentNode);
            },
            [](void* instance, const Document::Node& documentNode)
            {
                WeakComponentBase* weakComponentBase = static_cast<WeakComponentBase*>(instance);
                UID uid;
                Serializer::Deserialize(uid, documentNode);
                weakComponentBase->SetUid(uid);
            }
        );
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor)
    : _componentDescriptor(componentDescriptor)
    , _uid(UID::INVALID_UID)
    {
        
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor, const std::shared_ptr<Component>& pointer)
    : _componentDescriptor(componentDescriptor)
    , _uid(UID::INVALID_UID)
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
        _uid = copy._uid;
        _pointer = copy._pointer;
        _componentDescriptor = copy._componentDescriptor;
        return *this;
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase& WeakComponentBase::operator = (const std::weak_ptr<Component>& pointer)
    {
        _pointer = pointer.lock();
        return *this;
    }

    /// @brief 
    /// @param other 
    /// @return 
    bool WeakComponentBase::operator==(const WeakComponentBase& other) const
    {
        return _uid == other._uid;
    }

    /// @brief 
    /// @return 
    std::shared_ptr<Component> WeakComponentBase::Lock() const
    {
        std::shared_ptr<Component> lock = _pointer.lock();
        if (lock == nullptr)
        {
            _pointer = WeakComponentMapping::Resolve(_uid);
            lock = _pointer.lock();
        }
        return lock;
    }

    /// @brief 
    /// @return 
    const UID& WeakComponentBase::GetUid() const
    {
        return _uid;
    }

    void WeakComponentBase::SetUid(const UID& uid)
    {
        _uid = uid;
    }

    ReflectionDescriptor* WeakComponentBase::GetComponentDescriptor() const
    {
        return _componentDescriptor;
    }

    /// @brief 
    /// @return 
    const UID& WeakComponentBase::GetForSerialization() const
    {
        std::shared_ptr<Component> lock = _pointer.lock();
        if (lock == nullptr)
        {
            return this->_uid;
        }
        return lock->GetUid();
    }
}
