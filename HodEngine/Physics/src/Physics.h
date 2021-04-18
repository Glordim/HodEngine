#pragma once

#include "Debug/PhysicsDebugWindow.h"

#include <Core/src/Singleton.h>

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
		class Physics : public CORE::Singleton<Physics>
		{
			friend class CORE::Singleton<Physics>;

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
