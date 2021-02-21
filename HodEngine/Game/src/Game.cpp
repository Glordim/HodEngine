#include "Game.h"

#include "Scene.h"

#include <DebugLayer/src/DebugLayer.h>

namespace HOD
{
	GAME::Game* CORE::Singleton<GAME::Game>::_instance = nullptr;

	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Game::Game() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Game::~Game()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Game::Init()
		{
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

			pDebugLayer->RegisterDebugWindow(&_actorDebugWindow);
			pDebugLayer->RegisterDebugWindow(&_gizmoDebugWindow);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Game::Clear()
		{
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

			pDebugLayer->UnregisterDebugWindow(&_actorDebugWindow);
			pDebugLayer->UnregisterDebugWindow(&_gizmoDebugWindow);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* Game::CreateScene()
		{
			Scene* pScene = new Scene();

			_scenes.push_back(pScene);

			return pScene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Game::Update(float dt)
		{
			for (Scene* pScene : _scenes)
			{
				pScene->Update(dt);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Game::DebugActor(Actor* pActor)
		{
			_actorDebugWindow.SetActor(pActor);
			_gizmoDebugWindow.SetActor(pActor);
		}
	}
}
