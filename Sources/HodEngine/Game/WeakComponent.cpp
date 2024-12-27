#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::game
{
    std::unordered_map<uint64_t, std::weak_ptr<Component>> WeakComponentMapping::_map;

    void WeakComponentMapping::Insert(uint64_t instanceId, std::weak_ptr<Component> component)
    {
        _map[instanceId] = component;
    }

    void WeakComponentMapping::Clear()
    {
        _map.clear();
    };

    std::shared_ptr<Component> WeakComponentMapping::Resolve(uint64_t instanceId)
    {
        auto it = _map.find(instanceId);
        if (it == _map.end())
        {
            return std::shared_ptr<Component>();
        }
        else
        {
            return it->second.lock();
        }
    }

    DESCRIBE_REFLECTED_CLASS(WeakComponentBase, reflectionDescriptor)
    {
        reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
            [](const void* instance, Document::Node& documentNode)
            {
                const WeakComponentBase* weakComponentBase = static_cast<const WeakComponentBase*>(instance);
                uint64_t instanceId = weakComponentBase->GetForSerialization();
                documentNode.SetUInt64(instanceId);
            },
            [](void* instance, const Document::Node& documentNode)
            {
                WeakComponentBase* weakComponentBase = static_cast<WeakComponentBase*>(instance);
                uint64_t instanceId = documentNode.GetUInt64();
                weakComponentBase->SetInstanceId(instanceId);
            }
        );
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor)
    : _componentDescriptor(componentDescriptor)
    , _instanceId(0)
    {
        
    }

    /// @brief 
    /// @param pointer 
    WeakComponentBase::WeakComponentBase(ReflectionDescriptor* componentDescriptor, const std::shared_ptr<Component>& pointer)
    : _componentDescriptor(componentDescriptor)
    , _instanceId(0)
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
        _instanceId = copy._instanceId;
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
        return _instanceId == other._instanceId;
    }

    /// @brief 
    /// @return 
    std::shared_ptr<Component> WeakComponentBase::Lock() const
    {
        std::shared_ptr<Component> lock = _pointer.lock();
        if (lock == nullptr)
        {
            _pointer = WeakComponentMapping::Resolve(_instanceId);
            lock = _pointer.lock();
        }
        return lock;
    }

    /// @brief 
    /// @return 
    uint64_t WeakComponentBase::GetInstanceId() const
    {
        return _instanceId;
    }

    /// @brief 
    /// @param uid 
    void WeakComponentBase::SetInstanceId(uint64_t instanceId)
    {
        _instanceId = instanceId;
    }

    /// @brief 
    /// @param pointer 
    void WeakComponentBase::SetPointer(std::shared_ptr<Component> pointer)
    {
        if (pointer == nullptr)
        {
            _instanceId = 0;
        }
        else
        {
            _instanceId = pointer->GetInstanceId();
        }
        _pointer = pointer;
    }

    /// @brief 
    /// @return 
    ReflectionDescriptor* WeakComponentBase::GetComponentDescriptor() const
    {
        return _componentDescriptor;
    }

    /// @brief 
    /// @return 
    uint64_t WeakComponentBase::GetForSerialization() const
    {
        std::shared_ptr<Component> lock = _pointer.lock();
        if (lock == nullptr)
        {
            return _instanceId;
        }
        return lock->GetInstanceId();
    }
}
