#include "Game.h"

#include "Scene.h"

#include <DebugLayer/src/DebugLayer.h>

namespace HOD
{
    GAME::Game* CORE::Singleton<GAME::Game>::_instance = nullptr;

    namespace GAME
    {
		Game::Game() : Singleton()
        {

        }

		Game::~Game()
        {
            Clear();
        }

        bool Game::Init()
        {
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();
			
			pDebugLayer->RegisterDebugWindow(&_actorDebugWindow);
			pDebugLayer->RegisterDebugWindow(&_gizmoDebugWindow);

            return true;
        }

        void Game::Clear()
        {
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

			pDebugLayer->UnregisterDebugWindow(&_actorDebugWindow);
			pDebugLayer->UnregisterDebugWindow(&_gizmoDebugWindow);
        }

        Scene* Game::CreateScene()
		{
            Scene* pScene = new Scene();

            _scenes.push_back(pScene);

            return pScene;
		}

        void Game::DestroyScene(Scene* pScene)
        {
            auto it = _scenes.begin();
            auto itEnd = _scenes.end();
            while (it != itEnd)
            {
                if (*it == pScene)
                {
                    _scenes.erase(it);
                    break;
                }
            }

            delete pScene;
        }

		void Game::Update(float dt)
		{
			for (Scene* pScene : _scenes)
			{
				pScene->update(dt);
			}
		}

		void Game::DebugActor(Actor* pActor)
		{
			_actorDebugWindow.SetActor(pActor);
			_gizmoDebugWindow.SetActor(pActor);
		}
    }
}
