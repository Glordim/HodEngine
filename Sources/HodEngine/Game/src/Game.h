#pragma once

#include <HodEngine/Core/Src/Singleton.h>

#include <vector>

namespace HOD
{
	namespace GAME
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Game : public Singleton<Game>
		{
			friend class Singleton<Game>;

		public:

			bool				Init();
			void				Clear();

			void				Update(float dt);

			Scene*				CreateScene();
			void				DestroyScene(Scene* pScene);

		protected:

								Game();
								~Game() override;

		private:

			std::vector<Scene*>	_scenes;
		};
	}
}