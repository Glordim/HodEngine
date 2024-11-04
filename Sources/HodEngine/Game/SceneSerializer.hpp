#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>

#include <vector>
#include <memory>

namespace hod::game
{
    class Entity;
    class Component;

    class HOD_GAME_API SceneSerializer
    {
    public:
        static bool SerializeEntities(const std::vector<std::shared_ptr<Entity>>& entities, Document::Node& entitiesNode);
        static bool SerializeEntity(const std::shared_ptr<Entity> entity, Document::Node& entityNode);

        static std::shared_ptr<Entity> InstantiateEntityFromDocumentNode(const Document::Node& entityNode, std::vector<std::shared_ptr<Entity>>& entities, std::vector<std::shared_ptr<Component>>& components);
    };
}
