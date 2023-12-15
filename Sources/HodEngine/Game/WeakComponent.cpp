#include "WeakComponent.hpp"

namespace hod::game
{
    std::map<UID, std::weak_ptr<Component>> WeakComponentMapping::_map;
}
