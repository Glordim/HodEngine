#include "HodEngine/Physics/Pch.hpp"
#include "Physics.hpp"

#include "Body.hpp"

#include <box2d/box2d.h>

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

			b2DestroyWorld(_worldId);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::Init()
		{
			b2WorldDef worldDef = b2DefaultWorldDef();
			b2WorldId worldId = b2CreateWorld(&worldDef);

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
			b2BodyDef bodyDef = b2DefaultBodyDef();
			b2BodyId bodyId = b2CreateBody(_worldId, &bodyDef);

			Body* body = new Body(bodyId);

			_bodies.push_back(body);

			return body;
		}

		/// @brief 
		/// @param body 
		void Physics::DeleteBody(Body* body)
		{
			auto it = std::find(_bodies.begin(), _bodies.end(), body);
			if (it != _bodies.end())
			{
				_bodies.erase(it); // todo swap and popback ?
				b2DestroyBody(body->GetB2Actor());
				delete body;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Update(float dt)
		{
			b2World_Step(_worldId, dt, 4);
	
			/*
			if (_debugDrawer != nullptr)
			{
				_b2World->DebugDraw();
			}
			*/
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

		/*
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
		*/
	}
}
