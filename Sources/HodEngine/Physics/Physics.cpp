#include "Physics.hpp"

#include "Body.hpp"

#include <box2d/b2_world.h>
#include <box2d/b2_math.h>
#include <box2d/b2_body.h>
#include <box2d/b2_draw.h>

namespace hod
{
	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		_SingletonConstructor(Physics)
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

			b2Vec2 defaultGravity(0.0f, -9.8f);

			_b2World = new b2World(defaultGravity);

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

		void Physics::DeleteBody(Body* body)
		{
			auto it = std::find(_bodies.begin(), _bodies.end(), body);
			if (it != _bodies.end())
			{
				_bodies.erase(it); // todo swap and popback ?
				_b2World->DestroyBody(body->GetB2Actor());
				delete body;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Update(float dt)
		{
			_b2World->Step(dt, _velocityIterations, _positionIterations);
	
			if (_debugDrawer != nullptr)
			{
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
		void Physics::SetDebugDrawer(b2Draw* debugDrawer)
		{
			_debugDrawer = debugDrawer;
			_b2World->SetDebugDraw(_debugDrawer);
			SetDebugDrawFlags(DebugDrawFlag::Shape, true);
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
