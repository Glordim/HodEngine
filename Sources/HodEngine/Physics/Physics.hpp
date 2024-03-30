#pragma once

#include <HodEngine/Core/Singleton.hpp>

#include <vector>

class b2World;
class b2Draw;

namespace hod
{
	class Vector2;

	namespace renderer
	{
		class RenderQueue;
	}

	namespace physics
	{
		class Body;
		class DebugDrawer;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct RaycastResult
		{
			Body* _bodyCollided = nullptr;
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Physics : public Singleton<Physics>
		{
			friend class Singleton<Physics>;

		public:

			enum DebugDrawFlag
			{
				Shape = 0,
				Join,
				AABB,
				Pair,
				CenterOfMass
			};

		protected:

								Physics();
								~Physics() override;

		public:

			bool				Init();
			void				Clear();

			Body*				CreateBody();
			void				DeleteBody(Body* body);

			void				Update(float dt);

			bool				Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result);

			void				SetDebugDrawer(b2Draw* debugDrawer);
			void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

		private:

			b2World*			_b2World = nullptr;

			int32_t				_velocityIterations = 8;
			int32_t				_positionIterations = 3;

			std::vector<Body*>	_bodies;
			b2Draw*				_debugDrawer = nullptr;
		};
	}
}
