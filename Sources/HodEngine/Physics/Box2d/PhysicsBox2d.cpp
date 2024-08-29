#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/PhysicsBox2d.hpp"
#include "HodEngine/Physics/Box2d/BodyBox2d.hpp"

#include <box2d/box2d.h>

namespace hod::physics
{
	/// @brief 
	/// @param  
	_SingletonOverrideConstructor(PhysicsBox2d)
	{

	}

	/// @brief 
	PhysicsBox2d::~PhysicsBox2d()
	{
		Clear();

		b2DestroyWorld(_worldId);
	}

	/// @brief 
	/// @return 
	bool PhysicsBox2d::Init()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		b2WorldId worldId = b2CreateWorld(&worldDef);

		return true;
	}

	/// @brief 
	/// @return 
	Body* PhysicsBox2d::CreateBody()
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		b2BodyId bodyId = b2CreateBody(_worldId, &bodyDef);

		BodyBox2d* body = new BodyBox2d(bodyId);

		_bodies.push_back(body);

		return body;
	}

	/// @brief 
	/// @param body 
	void PhysicsBox2d::DeleteBody(Body* body)
	{
		auto it = std::find(_bodies.begin(), _bodies.end(), body);
		if (it != _bodies.end())
		{
			_bodies.erase(it); // todo swap and popback ?
			b2DestroyBody(static_cast<BodyBox2d*>(body)->GetB2Actor());
			delete body;
		}
	}

	/// @brief 
	/// @param dt 
	void PhysicsBox2d::Update(float dt)
	{
		b2World_Step(_worldId, dt, 4);

		/*
		if (_debugDrawer != nullptr)
		{
			_b2World->DebugDraw();
		}
		*/
	}

	/// @brief 
	/// @param origin 
	/// @param dir 
	/// @param distance 
	/// @param result 
	/// @return 
	bool PhysicsBox2d::Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result)
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
