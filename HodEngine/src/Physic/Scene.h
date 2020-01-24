#pragma once

#include <PxPhysicsAPI.h>

#include <glm/glm.hpp>

#include <vector>

namespace HOD
{
	struct Line_3P_3C;
	struct Tri_3P_3C;
	struct Color;

    namespace PHYSIC
    {
		class Actor;

		struct RaycastResult
		{

		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Scene
        {
        public:
								Scene(physx::PxScene* pPxScene);
								~Scene();

			Actor*				CreateActor();

			void				GetDebugGeometry(std::vector<Line_3P_3C>& lines, std::vector<Tri_3P_3C>& tris);

			void				Update(float dt);

			bool				Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result);

        private:
            physx::PxScene*		pxScene = nullptr;
			physx::PxMaterial*	pxDefaultMaterial = nullptr;

			std::vector<Actor*>	actors;
        };
    }
}
