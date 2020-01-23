#pragma once

#include <PxPhysicsAPI.h>

#include <vector>

namespace HOD
{
	struct Line_3P_3C;
	struct Tri_3P_3C;

    namespace PHYSIC
    {
		class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Scene
        {
        public:
            Scene(physx::PxScene* pPxScene);
            ~Scene();

			Actor*	CreateActor();

			void	GetDebugGeometry(std::vector<Line_3P_3C>& lines, std::vector<Tri_3P_3C>& tris);

			void	Update(float dt);

        private:
            physx::PxScene* pxScene = nullptr;
			physx::PxMaterial* pxDefaultMaterial = nullptr;

			std::vector<Actor*> actors;
        };
    }
}
