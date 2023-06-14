#include "HodEngine/Game/Entity.h"

namespace hod::game
{
    Entity::Id Entity::GetId() const
    {
        return _id;
    }
    const std::string& Entity::GetName() const
    {
        return _name;
    }
    bool Entity::GetActive() const
    {
        return _active;
    }
}