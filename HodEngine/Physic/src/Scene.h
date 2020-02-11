#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace physx
{
    class PxScene;
    class PxMaterial;
}

namespace HOD
{
	struct Line_3P_3C;
	struct Tri_3P_3C;
	struct Color;

    namespace PHYSIC
    {
		class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct RaycastResult
		{
            Actor*				_pActorCollided = nullptr;
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
            physx::PxScene*		_pPxScene = nullptr;
			physx::PxMaterial*	_pPxDefaultMaterial = nullptr;

			std::vector<Actor*>	_vActors;

		// Debug
		public:

			void				ApplyShapeVisualizationFlag(bool bVisualizeShape);
			void				ApplyActorVisualizationFlag(bool bVisualizeActor);

		};
    }
}
