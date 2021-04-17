#pragma once

#include <glm/glm.hpp>

#include <vector>

class b2World;

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
								Scene();
								~Scene();

			Actor*				CreateBody();

			void				GetDebugGeometry(std::vector<RENDERER::Line_3P_3C>& lines, std::vector<RENDERER::Tri_3P_3C>& tris);

			void				Update(float dt);

			bool				Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result);

			// Debug
		public:

			void				ApplyShapeVisualizationFlag(bool visualization);
			void				ApplyActorVisualizationFlag(bool visualization);

		private:

			b2World*			_world = nullptr;

			int32_t				_velocityIterations = 8;
			int32_t				_positionIterations = 3;

			std::vector<Actor*>	_actors;
		};
	}
}
