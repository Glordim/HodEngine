#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WeakEntity.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::game
{
    std::map<uint64_t, std::weak_ptr<Entity>> WeakEntityMapping::_map;

    void WeakEntityMapping::Insert(uint64_t id, std::weak_ptr<Entity> entity)
    {
        _map[id] = entity;
    }

    void WeakEntityMapping::Clear()
    {
        _map.clear();
    };

    std::shared_ptr<Entity> WeakEntityMapping::Resolve(uint64_t id)
    {
        auto it = _map.find(id);
        if (it == _map.end())
        {
            return std::shared_ptr<Entity>();
        }
        else
        {
            return it->second.lock();
        }
    }

    DESCRIBE_REFLECTED_CLASS(WeakEntity, reflectionDescriptor)
    {
        reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
            [](const void* instance, Document::Node& documentNode)
            {
                const WeakEntity* weakEntity = static_cast<const WeakEntity*>(instance);
                uint64_t id = weakEntity->GetForSerialization();
                documentNode.AddChild("Id").SetUInt64(id);
                //Serializer::Serialize(id, documentNode);
            },
            [](void* instance, const Document::Node& documentNode)
            {
                WeakEntity* weakEntity = static_cast<WeakEntity*>(instance);
                uint64_t id;
                //Serializer::Deserialize(id, documentNode);
                const Document::Node* idNode = documentNode.GetChild("Id");
                if (idNode != nullptr)
                {
                    weakEntity->SetId(idNode->GetUInt64());
                }
                else
                {
                    // todo message
                }
            }
        );
    }

    /// @brief 
    /// @param pointer 
    WeakEntity::WeakEntity()
    : _id(0)
    {
        
    }

    /// @brief 
    /// @param pointer 
    WeakEntity::WeakEntity(const std::shared_ptr<Entity>& pointer)
    : _id(0)
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
        _id = copy._id;
        _pointer = copy._pointer;
        return *this;
    }

    /// @brief 
    /// @param pointer 
    WeakEntity& WeakEntity::operator = (const std::weak_ptr<Entity>& pointer)
    {
        _pointer = pointer.lock();
        return *this;
    }

    /// @brief 
    /// @param other 
    /// @return 
    bool WeakEntity::operator==(const WeakEntity& other) const
    {
        return _id == other._id;
    }

    /// @brief 
    /// @return 
    std::shared_ptr<Entity> WeakEntity::Lock() const
    {
        std::shared_ptr<Entity> lock = _pointer.lock();
        if (lock == nullptr)
        {
            _pointer = WeakEntityMapping::Resolve(_id);
            lock = _pointer.lock();
        }
        return lock;
    }

    /// @brief 
    /// @return 
    uint64_t WeakEntity::GetId() const
    {
        return _id;
    }

    /// @brief 
    /// @param uid 
    void WeakEntity::SetId(uint64_t id)
    {
        _id = id;
    }

    /// @brief 
    /// @param pointer 
    void WeakEntity::SetPointer(std::shared_ptr<Entity> pointer)
    {
        _id = 0;
        _pointer = pointer;
    }

    /// @brief 
    /// @return 
    uint64_t WeakEntity::GetForSerialization() const
    {
        std::shared_ptr<Entity> lock = _pointer.lock();
        if (lock == nullptr)
        {
            return _id;
        }
        return lock->GetId();
    }
}
