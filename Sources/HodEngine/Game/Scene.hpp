#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::game
{
	class RendererComponent;
	class Prefab;
	class Entity;
	class Component;

	/// @brief 
	class Scene
	{
		REFLECTED_CLASS_NO_PARENT(Scene)

	public:
						Scene() = default;
						Scene(const Scene&) = delete;
						Scene(Scene&&) = delete;
		virtual			~Scene() = default;

		Scene&			operator=(const Scene&) = delete;
		Scene&			operator=(Scene&&) = delete;

	public:

		void				SetName(const std::string_view& name);
		const std::string&	GetName() const;

		bool			SerializeInDocument(Document::Node& documentNode) const;
		bool			DeserializeFromDocument(const Document::Node& documentNode);

		std::weak_ptr<Entity>			CreateEntity(const std::string_view& name = "");
		void							DestroyEntity(std::shared_ptr<Entity> entity);
		std::weak_ptr<Entity>			FindEntity(Entity::Id entityId);

		Scene*							Clone();
		void							Clear();

		std::shared_ptr<Entity>			Instantiate(Prefab& prefab);
		std::shared_ptr<Entity>			Instantiate(std::shared_ptr<Entity> entity);

		std::shared_ptr<Entity>			InstantiateWithOverrides(Prefab& prefab, const Document::Node& prefabNode);

		const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

		void						Awake();
		void						Start();
		void						Update();
		void						Draw(renderer::RenderQueue* renderQueue);
		void						DrawPicking(renderer::RenderQueue* renderQueue, std::map<uint32_t, std::shared_ptr<RendererComponent>>& colorIdToRendererComponentMap, uint32_t& id);

	private:

		std::shared_ptr<Entity>		InstantiateInternal(std::shared_ptr<Entity> entity, std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>>& sourceToCloneEntitiesMap, std::unordered_map<std::shared_ptr<Component>, std::shared_ptr<Component>>& sourceToCloneComponentsMap);

	private:

		std::string												_name;
		std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;
	};
}
