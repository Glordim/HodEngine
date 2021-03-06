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
	namespace CORE
	{
		struct Color;
	}

	namespace RENDERER
	{
		struct Line_3P_3C;
		struct Tri_3P_3C;
	}

	namespace PHYSICS
	{
		class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct RaycastResult
		{
			Actor* _actorCollided = nullptr;
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Scene
		{
		public:
								Scene(physx::PxScene* pxScene);
								~Scene();

			Actor*				CreateActor();

			void				GetDebugGeometry(std::vector<RENDERER::Line_3P_3C>& lines, std::vector<RENDERER::Tri_3P_3C>& tris);

			void				Update(float dt);

			bool				Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result);

			// Debug
		public:

			void				ApplyShapeVisualizationFlag(bool visualization);
			void				ApplyActorVisualizationFlag(bool visualization);

		private:
			physx::PxScene*		_pxScene = nullptr;
			physx::PxMaterial*	_pxDefaultMaterial = nullptr;

			std::vector<Actor*>	_actors;
		};
	}
}
