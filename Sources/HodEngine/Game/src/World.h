#pragma once

#include <HodEngine/Core/Singleton.h>

#include "HodEngine/Game/Entity.h"

#include <vector>
#include <unordered_map>
#include <memory>

#include "HodEngine/Core/Event.h"

namespace hod
{
	namespace game
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class World : public Singleton<World>
		{
			friend class Singleton<World>;

		public:

			bool				Init();
			void				Clear();

			void				Update(float dt);

			Scene*				CreateScene();
			void				DestroyScene(Scene* pScene);

			bool				AddScene(Scene* scene);
			bool				RemoveScene(Scene* scene);

			const std::vector<Scene*>&	GetScenes() const;

			std::weak_ptr<Entity> CreateEntity(const std::string_view& name = "");
			const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

			std::weak_ptr<Entity> FindEntity(Entity::Id entityId);

			Event<std::weak_ptr<Entity>>&	GetAddEntityEvent() { return _addEntityEvent; }
			Event<std::weak_ptr<Entity>>&	GetRemoveEntityEvent() { return _removeEntityEvent; }
			Event<std::weak_ptr<Entity>>&	GetRenameEntityEvent() { return _renameEntityEvent; }

		protected:

								World();
								~World() override;

		private:

			std::vector<Scene*>	_scenes;
			std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;

			Event<std::weak_ptr<Entity>>	_addEntityEvent;
			Event<std::weak_ptr<Entity>>	_removeEntityEvent;
			Event<std::weak_ptr<Entity>>	_renameEntityEvent;
		};
	}
}
