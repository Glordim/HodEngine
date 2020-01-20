#pragma once

#include <PxPhysicsAPI.h>

#include <vector>

namespace HOD
{
    namespace PHYSIC
    {
		class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Scene
        {
        public:
            Scene() = default;
            ~Scene() = default;

			Actor*	CreateActor();

			void	Update(float dt);

        private:
            physx::PxScene* pxScene = nullptr;
			physx::PxMaterial* pxDefaultMaterial = nullptr;

			std::vector<Actor*> actors;
        };
    }
}
