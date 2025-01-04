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
        static bool SerializeEntity(const std::shared_ptr<Entity> entity, bool withChildren, Document::Node& entityNode, uint64_t& nextLocalId);

        static std::shared_ptr<Entity> InstantiateEntityFromDocumentNode(const Document::Node& entityNode, std::unordered_map<uint64_t, std::shared_ptr<Entity>>& entities, std::unordered_map<uint64_t, std::shared_ptr<Component>>& components);
    };
}
