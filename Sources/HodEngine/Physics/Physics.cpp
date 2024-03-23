#include "Physics.hpp"

#include "Body.hpp"

#include "DebugDrawer.hpp"

#include <box2d/b2_world.h>
#include <box2d/b2_math.h>
#include <box2d/b2_body.h>

namespace hod
{
	template<>
	physics::Physics* Singleton<physics::Physics>::_instance = nullptr;

	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::Physics() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::~Physics()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::Init()
		{
			_debugDrawer = new DebugDrawer();

			b2Vec2 defaultGravity(0.0f, -9.8f);

			_b2World = new b2World(defaultGravity);
			_b2World->SetDebugDraw(_debugDrawer);
			SetDebugDrawFlags(DebugDrawFlag::Shape, true);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Clear()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Body* Physics::CreateBody()
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_staticBody;

			Body* body = new Body(_b2World->CreateBody(&bodyDef));

			_bodies.push_back(body);

			return body;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Update(float dt)
		{
			_b2World->Step(dt, _velocityIterations, _positionIterations);
	
			if (_useDebugDraw == true)
			{
				assert(_debugDrawer->HasRenderCommand() == false);
				_b2World->DebugDraw();
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result)
		{
			// TODO
			//_world->RayCast();
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			if (_useDebugDraw == true)
			{
				_debugDrawer->PushToRenderQueue(renderQueue);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::SetDebugDraw(bool debugDraw)
		{
			_useDebugDraw = debugDraw;
		}
		
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::SetDebugDrawFlags(DebugDrawFlag flag, bool enabled)
		{
			uint32_t mask = _debugDrawer->GetFlags();

			if (enabled == true)
			{
				mask |= (1 << flag);
			}
			else
			{
				mask &= ~(1 << flag);
			}

			_debugDrawer->SetFlags(mask);
		}
	}
}
