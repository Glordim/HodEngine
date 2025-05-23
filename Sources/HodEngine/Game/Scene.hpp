#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::renderer
{
	class RenderQueue;
}

namespace hod::game
{
	class RendererComponent;
	class PrefabResource;
	class Entity;
	class Component;
	class World;

	/// @brief 
	class HOD_GAME_API Scene
	{
		REFLECTED_CLASS_NO_PARENT(Scene)

	public:
						Scene();
						Scene(const Scene&) = delete;
						Scene(Scene&&) = delete;
		virtual			~Scene() = default;

		Scene&			operator=(const Scene&) = delete;
		Scene&			operator=(Scene&&) = delete;

	public:

		void				SetName(const std::string_view& name);
		const std::string&	GetName() const;

		bool				SerializeInDocument(Document::Node& documentNode);
		bool				DeserializeFromDocument(const Document::Node& documentNode);

		Entity*				CreateEntity(const std::string_view& name = "");
		void				DestroyEntity(Entity* entity);
		Entity*				FindEntity(uint64_t entityId);

		Scene*							Clone();
		void							Clear();

		Entity*			Instantiate(std::shared_ptr<PrefabResource> prefabResource);
		Entity*			Instantiate(Entity* entity);

		const std::unordered_map<uint64_t, Entity*>& GetEntities() const;

		void						ProcessActivation();
		void						Update(float deltaTime);
		void						Draw(renderer::RenderQueue* renderQueue);

		void						SetNextLocalId(uint64_t nextLocalId) { _nextLocalId = nextLocalId; }

		uint64_t					GetNextLocalId() const { return _nextLocalId; }

		void						SetWorld(World* world);
		World*						GetWorld() const;

	private:

		std::string												_name;
		std::unordered_map<uint64_t, Entity*>	_entities;
		uint64_t												_nextLocalId = 1;
		World*													_world = nullptr;
	};
}
