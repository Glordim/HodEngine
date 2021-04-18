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
		class DebugDrawer;

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

			enum DebugDrawFlag
			{
				Shape = 0,
				Join,
				AABB,
				Pair,
				CenterOfMass
			};

		public:
								Scene();
								~Scene();

			Actor*				CreateBody();

			void				GetDebugGeometry(std::vector<RENDERER::Line_3P_3C>& lines, std::vector<RENDERER::Tri_3P_3C>& tris);

			void				Update(float dt);

			bool				Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result);

		// Debug
		public:

			void				SetDebugDraw(bool debugDraw);
			void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

		private:

			b2World*			_b2World = nullptr;

			bool				_useDebugDraw = false;
			DebugDrawer*		_debugDrawer = nullptr;

			int32_t				_velocityIterations = 8;
			int32_t				_positionIterations = 3;

			std::vector<Actor*>	_actors;
		};
	}
}
