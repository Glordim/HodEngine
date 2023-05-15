#pragma once

#include <HodEngine/Core/Singleton.h>

#include <vector>

namespace hod
{
	namespace physics
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
		};
	}
}
