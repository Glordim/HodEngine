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

        protected:

                                    Game();
                                    ~Game() override;

        public:

            bool                    Init();
            void                    Clear();

			void					Update(float dt);

			Scene*					CreateScene();
            void                    DestroyScene(Scene* pScene);

        private:

            std::vector<Scene*>		_scenes;

		//Debug
		public:

			void					DebugActor(Actor* pActor);

		private:

			ActorDebugWindow		_actorDebugWindow;
			GizmoDebugWindow		_gizmoDebugWindow;
        };
    }
}
