#include "Actor.h"

#include "Physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_polygon_shape.h>

#include <HodEngine/Renderer/BoundingBox.h>

#include <algorithm>

namespace hod
{
	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::Actor(b2Body* b2Body) : _b2Body(b2Body)
		{
			//_pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, Physics::GetInstance()->GetActorVisualizationFlag());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::AddEdgeShape(const glm::vec2& startPosition, const glm::vec2& endPosition)
		{
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(startPosition.x, startPosition.y), b2Vec2(endPosition.x, endPosition.y));
			shape.m_radius *= 0.25f;

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::AddCircleShape(const glm::vec2& position, float radius)
		{
			b2CircleShape shape;
			shape.m_p.x = position.x;
			shape.m_p.y = position.y;
			shape.m_radius = radius;

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::AddBoxShape(const glm::vec2& position, const glm::vec2& size, float angle, float density)
		{
			b2PolygonShape shape;
			shape.SetAsBox(size.x, size.y, b2Vec2(position.x, position.y), angle);
			shape.m_radius *= 0.25f;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = density;

			_b2Body->CreateFixture(&fixtureDef);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::AddConvexShape(const std::vector<const glm::vec2>& vertices)
		{
			b2PolygonShape shape;
			// todo
			//shape.Set(vertices.data(), vertices.size());

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetTransform(const glm::vec2& position, float rotation, const glm::vec2& scale)
		{
			// todo scale
			_b2Body->SetTransform(b2Vec2(position.x, position.y), glm::radians<float>(rotation));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::vec2 Actor::GetPosition() const
		{
			return glm::vec2(_b2Body->GetPosition().x, _b2Body->GetPosition().y);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float Actor::GetRotation() const
		{
			return glm::degrees(_b2Body->GetAngle());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::TYPE Actor::GetType() const
		{
			return static_cast<TYPE>(_b2Body->GetType());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetType(TYPE type)
		{
			_b2Body->SetType(static_cast<b2BodyType>(type));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		b2Body* Actor::GetB2Actor() const
		{
			return _b2Body;
		}
	}
}
