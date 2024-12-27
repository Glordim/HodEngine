#pragma once
#include "HodEngine/Game/Export.hpp"
#include <memory>
#include <map>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp>

namespace hod::game
{
    class Entity;

    /// @brief 
    class HOD_GAME_API WeakEntityMapping
    {
    public:

        static void Insert(uint64_t id, std::weak_ptr<Entity> entity);
        static void Clear();

        static std::shared_ptr<Entity> Resolve(uint64_t id);

    private:

        static std::map<uint64_t, std::weak_ptr<Entity>> _map;
    };

    class HOD_GAME_API WeakEntity
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakEntity)

    public:

        public:

                            WeakEntity() = default;
                            WeakEntity(const std::shared_ptr<Entity>& pointer);
        virtual             ~WeakEntity();

        WeakEntity&         operator = (const WeakEntity& copy);
        WeakEntity&         operator = (const std::weak_ptr<Entity>& pointer);

        bool                operator==(const WeakEntity& other) const;

    public:

        std::shared_ptr<Entity>     Lock() const;
        uint64_t                    GetInstanceId() const;
        uint64_t                    GetForSerialization() const;

        ReflectionDescriptor*       GetComponentDescriptor() const;

        void                        SetInstanceId(uint64_t id);
        void                        SetPointer(std::shared_ptr<Entity> pointer);

    private:
        
        uint64_t                            _instanceId = 0;
        mutable std::weak_ptr<Entity>       _pointer;
    };
}
