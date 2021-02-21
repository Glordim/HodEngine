#pragma once

#include "Debug/ActorDebugWindow.h"
#include "Debug/GizmoDebugWindow.h"

#include <Core/src/Singleton.h>

#include <vector>

namespace HOD
{
	namespace GAME
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Game : public CORE::Singleton<Game>
		{
			friend class CORE::Singleton<Game>;

		public:

			bool				Init();
			void				Clear();

			void				Update(float dt);

			Scene*				CreateScene();
			void				DestroyScene(Scene* pScene);

			void				DebugActor(Actor* pActor);

		protected:

								Game();
								~Game() override;

		private:

			std::vector<Scene*>	_scenes;

			ActorDebugWindow	_actorDebugWindow;
			GizmoDebugWindow	_gizmoDebugWindow;
		};
	}
}
