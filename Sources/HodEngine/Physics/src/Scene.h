#pragma once

#include <glm/glm.hpp>

#include <vector>

class b2World;

namespace hod
{
	namespace CORE
	{
		struct Color;
	}

	namespace renderer
	{
		class RenderQueue;
	}

	namespace physics
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

			void				Update(float dt);

			void				PushToRenderQueue(renderer::RenderQueue& renderQueue);

			bool				Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, physics::RaycastResult& result);

		// Debug
		public:

			void				SetDebugDraw(bool debugDraw);
			void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

		private:

			b2World*			_b2World = nullptr;

			bool				_useDebugDraw = true;
			DebugDrawer*		_debugDrawer = nullptr;

			int32_t				_velocityIterations = 8;
			int32_t				_positionIterations = 3;

			std::vector<Actor*>	_actors;
		};
	}
}
