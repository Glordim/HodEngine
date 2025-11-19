#pragma once
#include "HodEngine/Game/Export.hpp"
#include <memory>
#include <map>

#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::game
{
    class Entity;

    class HOD_GAME_API WeakEntity
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakEntity)

    public:

        public:

                            WeakEntity() = default;
                            WeakEntity(Entity* pointer);
        virtual             ~WeakEntity();

        WeakEntity&         operator = (const WeakEntity& copy);
        WeakEntity&         operator = (Entity* pointer);

        bool                operator==(const WeakEntity& other) const;

    public:

        Entity*                     Lock() const;
        uint64_t                    GetInstanceId() const;
        uint64_t                    GetForSerialization() const;

        ReflectionDescriptor*       GetComponentDescriptor() const;

        void                        SetInstanceId(uint64_t id);
        void                        SetPointer(Entity* pointer);

    private:
        
        uint64_t                            _instanceId = 0;
        mutable WeakPtr<Entity>             _pointer;
    };
}
