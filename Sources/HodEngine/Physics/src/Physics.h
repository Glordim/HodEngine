#pragma once

#include "Debug/PhysicsDebugWindow.h"

#include <HodEngine/Core/Src/Singleton.h>

#include <vector>

namespace HOD
{
	namespace PHYSICS
	{
		class Actor;
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Physics : public Singleton<Physics>
		{
			friend class Singleton<Physics>;

		protected:

											Physics();
											~Physics() override;

		public:

			bool							Init();
			void							Clear();

			Scene*							CreateScene();
			void							DestroyScene(Scene* scene);

		private:

			std::vector<Scene*>				_scenes;

		private:

			PhysicsDebugWindow				_physicDebugWindow;
		};
	}
}
