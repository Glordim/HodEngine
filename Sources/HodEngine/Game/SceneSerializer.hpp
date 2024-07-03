#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Core/Document/Document.hpp>

#include <vector>
#include <memory>

namespace hod::game
{
    class Entity;

    class HOD_API SceneSerializer
    {
    public:
        static bool SerializeEntities(const std::vector<std::shared_ptr<Entity>>& entities, Document::Node& entitiesNode);
    };
}
