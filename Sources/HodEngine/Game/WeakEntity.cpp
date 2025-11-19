#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WeakEntity.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::game
{
    DESCRIBE_REFLECTED_CLASS(WeakEntity, reflectionDescriptor)
    {
        reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
            [](const void* instance, Document::Node& documentNode)
            {
                const WeakEntity* weakEntity = static_cast<const WeakEntity*>(instance);
                uint64_t id = weakEntity->GetForSerialization();
                documentNode.AddChild("Id").SetUInt64(id);
                //Serializer::Serialize(id, documentNode);
                return true;
            },
            [](void* instance, const Document::Node& documentNode)
            {
                WeakEntity* weakEntity = static_cast<WeakEntity*>(instance);
                //uint64_t id;
                //Serializer::Deserialize(id, documentNode);
                const Document::Node* idNode = documentNode.GetChild("Id");
                if (idNode != nullptr)
                {
                    weakEntity->SetInstanceId(idNode->GetUInt64());
                }
                else
                {
                    // todo message
                }

                return true;
            }
        );
    }

    /// @brief 
    /// @param pointer 
    WeakEntity::WeakEntity(Entity* pointer)
    : _instanceId(0)
    , _pointer(pointer)
    {
    }

    /// @brief 
    WeakEntity::~WeakEntity()
    {
    }

    /// @brief 
    /// @param pointer 
    WeakEntity& WeakEntity::operator = (const WeakEntity& copy)
    {
        _instanceId = copy._instanceId;
        _pointer = copy._pointer;
        return *this;
    }

    /// @brief 
    /// @param pointer 
    WeakEntity& WeakEntity::operator = (Entity* pointer)
    {
        _pointer = pointer;
        return *this;
    }

    /// @brief 
    /// @param other 
    /// @return 
    bool WeakEntity::operator==(const WeakEntity& other) const
    {
        return _instanceId == other._instanceId;
    }

    /// @brief 
    /// @return 
    Entity* WeakEntity::Lock() const
    {
        return _pointer.Get();
    }

    /// @brief 
    /// @return 
    uint64_t WeakEntity::GetInstanceId() const
    {
        return _instanceId;
    }

    /// @brief 
    /// @param uid 
    void WeakEntity::SetInstanceId(uint64_t instanceId)
    {
        _instanceId = instanceId;
    }

    /// @brief 
    /// @param pointer 
    void WeakEntity::SetPointer(Entity* pointer)
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
    uint64_t WeakEntity::GetForSerialization() const
    {
        if (_pointer.Get() == nullptr)
        {
            return 0;
        }
        return _pointer->GetLocalId();
    }
}
