#pragma once

#include <HodEngine/Core/Singleton.h>

#include "HodEngine/Game/Entity.h"

#include <vector>
#include <unordered_map>

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

			Entity*				CreateEntity(const std::string_view& name = "");
			Entity*				GetEntities();

		protected:

								World();
								~World() override;

		private:

			std::vector<Scene*>	_scenes;
			std::unordered_map<Entity::Id, Entity>	_entities;
		};
	}
}
