#pragma once

#include <cstdint>
#include <string>

namespace hod::game
{
    class Entity final
    {
    public:

        using Id = uint64_t;

    public:

                        Entity() = default;
                        Entity(const Entity&) = delete;
                        Entity(Entity&&) = delete;
                        ~Entity() = default;

        const Entity&   operator = (const Entity&) = delete;
        const Entity&   operator = (Entity&&) = delete;

    public:

        Id                  GetId() const;
        const std::string&  GetName() const;
        bool                GetActive() const;

    private:

        Id              _id;
        std::string     _name;
        bool            _active;
    };
}