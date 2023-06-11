#pragma once

#include <HodEngine/Core/Singleton.h>

#include <vector>

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

		protected:

								World();
								~World() override;

		private:

			std::vector<Scene*>	_scenes;
		};
	}
}
