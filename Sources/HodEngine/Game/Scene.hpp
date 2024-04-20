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

		const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

		void						Awake();
		void						Start();
		void						Update();
		void						Draw(renderer::RenderQueue* renderQueue);
		void						DrawPicking(renderer::RenderQueue* renderQueue, std::map<uint32_t, std::shared_ptr<RendererComponent>>& colorIdToRendererComponentMap, uint32_t& id);

	private:

		std::string												_name;
		std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;
	};
}
