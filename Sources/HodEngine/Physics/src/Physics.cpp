#include "Physics.h"

#include "Scene.h"
#include "Actor.h"

#include <HodEngine/DebugLayer/src/DebugLayer.h>

namespace HOD
{
	template<>
	PHYSICS::Physics* Singleton<PHYSICS::Physics>::_instance = nullptr;

	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::Physics() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::~Physics()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::Init()
		{
			DEBUG_LAYER::DebugLayer::GetInstance()->RegisterDebugWindow(&_physicDebugWindow);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Clear()
		{
			DEBUG_LAYER::DebugLayer::GetInstance()->UnregisterDebugWindow(&_physicDebugWindow);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* Physics::CreateScene()
		{
			Scene* scene = new Scene();

			_scenes.push_back(scene);

			return scene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::DestroyScene(Scene* scene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					_scenes.erase(it);
					break;
				}
			}

			delete scene;
		}
	}
}
