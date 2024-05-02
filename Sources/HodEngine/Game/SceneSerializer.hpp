#pragma once

#include <HodEngine/Core/Document/Document.hpp>

#include <vector>
#include <memory>

namespace hod::game
{
    class Entity;

    class SceneSerializer
    {
    public:
        static bool SerializeEntities(const std::vector<std::shared_ptr<Entity>>& entities, Document::Node& entitiesNode);
    };
}
