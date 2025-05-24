#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/WorldBox2d.hpp"
#include "HodEngine/Physics/Box2d/BodyBox2d.hpp"

#include <box2d/box2d.h>

namespace hod::physics
{
	/// @brief 
	/// @param body 
	WorldBox2d::WorldBox2d()
	: World()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity.y = -9.8f;
		_worldId = b2CreateWorld(&worldDef);

		_debugDrawer = new DebugDrawerBox2d();
	}

	/// @brief 
	WorldBox2d::~WorldBox2d()
	{
		Clear();

		delete _debugDrawer;
		_debugDrawer = nullptr;

		b2DestroyWorld(_worldId);
	}

	/// @brief 
	/// @return 
	Body* WorldBox2d::CreateBody(Body::Type type, const Vector2& position, float rotation)
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = static_cast<b2BodyType>(type);
		bodyDef.position = { position.GetX(), position.GetY() };
		bodyDef.rotation = b2MakeRot(rotation);
		bodyDef.enableSleep = true;
		bodyDef.isAwake = true;
		bodyDef.fixedRotation = true;
		bodyDef.isEnabled = false;
		b2BodyId bodyId = b2CreateBody(_worldId, &bodyDef);

		b2MassData myMassData;
		myMassData.mass = 10.0f;
		myMassData.center = { 0.0f, 0.0f };
		myMassData.rotationalInertia = 100.0f;
		b2Body_SetMassData(bodyId, myMassData);

		BodyBox2d* body = new BodyBox2d(bodyId);

		b2Body_ApplyMassFromShapes(bodyId);

		_bodies.push_back(body);

		return body;
	}

	/// @brief 
	/// @param body 
	void WorldBox2d::DeleteBody(Body* body)
	{
		auto it = std::find(_bodies.begin(), _bodies.end(), body);
		if (it != _bodies.end())
		{
			_bodies.erase(it); // todo swap and popback ?
			b2BodyId bodyId =  static_cast<BodyBox2d*>(body)->GetB2Actor();
			delete body;
			b2DestroyBody(bodyId);
		}
	}

	/// @brief 
	/// @param dt 
	void WorldBox2d::Update(float dt)
	{
		b2World_Step(_worldId, dt, 4);

		b2BodyEvents bodyEvents = b2World_GetBodyEvents(_worldId);
		for (int32_t index = 0; index < bodyEvents.moveCount; ++index)
		{
			const b2BodyMoveEvent& moveEvent = bodyEvents.moveEvents[index];

			BodyBox2d* body = static_cast<BodyBox2d*>(moveEvent.userData);
			Vector2 position(moveEvent.transform.p.x, moveEvent.transform.p.y);
			float angle = math::RadianToDegree(b2Rot_GetAngle(moveEvent.transform.q));
			body->GetMoveEventCallback()(position, angle);
		}

		b2SensorEvents sensorEvents = b2World_GetSensorEvents(_worldId);
		for (int32_t index = 0; index < sensorEvents.beginCount; ++index)
		{
			const b2SensorBeginTouchEvent& beginEvent = sensorEvents.beginEvents[index];
			Collider* sensor = static_cast<Collider*>(b2Shape_GetUserData(beginEvent.sensorShapeId));
			Collider* visitor = static_cast<Collider*>(b2Shape_GetUserData(beginEvent.visitorShapeId));
			BodyBox2d* sensorBody = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(beginEvent.sensorShapeId)));
			BodyBox2d* visitorBody = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(beginEvent.visitorShapeId)));
			sensorBody->GetTriggerEnterCallback()(*sensor, *visitor);
			visitorBody->GetTriggerEnterCallback()(*sensor, *visitor);
		}
		for (int32_t index = 0; index < sensorEvents.endCount; ++index)
		{
			const b2SensorEndTouchEvent& endEvent = sensorEvents.endEvents[index];
			Collider* sensor = static_cast<Collider*>(b2Shape_GetUserData(endEvent.sensorShapeId));
			Collider* visitor = static_cast<Collider*>(b2Shape_GetUserData(endEvent.visitorShapeId));
			BodyBox2d* sensorBody = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(endEvent.sensorShapeId)));
			BodyBox2d* visitorBody = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(endEvent.visitorShapeId)));
			sensorBody->GetTriggerExitCallback()(*sensor, *visitor);
			visitorBody->GetTriggerExitCallback()(*sensor, *visitor);
		}

		b2ContactEvents contactEvents = b2World_GetContactEvents(_worldId);
		for (int32_t index = 0; index < contactEvents.beginCount; ++index)
		{
			const b2ContactBeginTouchEvent& beginEvent = contactEvents.beginEvents[index];
			Collision collision {
				*static_cast<Collider*>(b2Shape_GetUserData(beginEvent.shapeIdA)),
				*static_cast<Collider*>(b2Shape_GetUserData(beginEvent.shapeIdB)),
			};
			b2ContactData contactData[10];
			int contactCount = b2Shape_GetContactData(beginEvent.shapeIdA, contactData, 10);
			for (int contactIndex = 0; contactIndex < contactCount; ++contactIndex)
			{
				b2ContactData& data = contactData[contactIndex];

				if (data.shapeIdA.index1 == beginEvent.shapeIdA.index1 &&
					data.shapeIdB.index1 == beginEvent.shapeIdB.index1)
				{
					collision._normal = Vector2(data.manifold.normal.x, data.manifold.normal.y);
					BodyBox2d* bodyA = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(beginEvent.shapeIdA)));
					BodyBox2d* bodyB = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(beginEvent.shapeIdB)));
					bodyA->GetCollisionEnterCallback()(collision);
					if (bodyB != bodyA)
					{
						bodyB->GetCollisionEnterCallback()(collision);
					}
					break;
				}
			}
		}
		for (int32_t index = 0; index < contactEvents.endCount; ++index)
		{
			const b2ContactEndTouchEvent& endEvent = contactEvents.endEvents[index];

			Collision collision {
				*static_cast<Collider*>(b2Shape_GetUserData(endEvent.shapeIdA)),
				*static_cast<Collider*>(b2Shape_GetUserData(endEvent.shapeIdB)),
			};
			BodyBox2d* bodyA = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(endEvent.shapeIdA)));
			BodyBox2d* bodyB = static_cast<BodyBox2d*>(b2Body_GetUserData(b2Shape_GetBody(endEvent.shapeIdB)));
			bodyA->GetCollisionExitCallback()(collision);
			if (bodyB != bodyA)
			{
				bodyB->GetCollisionExitCallback()(collision);
			}
		}
	}

	/// @brief 
	/// @param origin 
	/// @param dir 
	/// @param distance 
	/// @param result 
	/// @return 
	bool WorldBox2d::Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result)
	{
		// TODO
		//_world->RayCast();
		return false;
	}

	/// @brief 
	/// @return 
	b2WorldId WorldBox2d::GetWorldId() const
	{
		return _worldId;
	}

	/// @brief 
	/// @param shapeId 
	/// @return 
	ColliderBox2d* WorldBox2d::FindColliderByB2ShapeId(b2ShapeId shapeId) const
	{
		for (uint32_t index = 0; index < _bodies.size(); ++index)
		{
			BodyBox2d* body = (BodyBox2d*)_bodies[index];
			ColliderBox2d* collider = body->FindColliderByB2ShapeId(shapeId);
			if (collider != nullptr)
			{
				return collider;
			}
		}
		return nullptr;
	}
}
