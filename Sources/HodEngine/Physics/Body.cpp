#include "HodEngine/Physics/Pch.hpp"
#include "Body.hpp"

#include "Physics.hpp"

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_polygon_shape.h>

#include <HodEngine/Renderer/BoundingBox.hpp>

#include <algorithm>

#include "HodEngine/Core/Math/Math.hpp"

namespace hod
{
	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Body::Body(b2Body* b2Body)
		: _b2Body(b2Body)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition)
		{
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(startPosition.GetX(), startPosition.GetY()), b2Vec2(endPosition.GetX(), endPosition.GetY()));
			shape.m_radius *= 0.25f;

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::AddCircleShape(const Vector2& position, float radius)
		{
			b2CircleShape shape;
			shape.m_p.x = position.GetX();
			shape.m_p.y = position.GetY();
			shape.m_radius = radius;

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density)
		{
			b2PolygonShape shape;
			shape.SetAsBox(size.GetX(), size.GetY(), b2Vec2(position.GetX(), position.GetY()), angle);
			shape.m_radius *= 0.25f;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = density;

			_b2Body->CreateFixture(&fixtureDef);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::AddConvexShape(const std::vector<const Vector2>& vertices)
		{
			b2PolygonShape shape;
			// todo
			//shape.Set(vertices.data(), vertices.size());

			_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::SetTransform(const Vector2& position, float rotation, const Vector2& scale)
		{
			// todo scale
			_b2Body->SetTransform(b2Vec2(position.GetX(), position.GetY()), math::DegreeToRadian(rotation));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Vector2 Body::GetPosition() const
		{
			return Vector2(_b2Body->GetPosition().x, _b2Body->GetPosition().y);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float Body::GetRotation() const
		{
			return math::RadianToDegree(_b2Body->GetAngle());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Body::Type Body::GetType() const
		{
			return static_cast<Type>(_b2Body->GetType());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Body::SetType(Type type)
		{
			_b2Body->SetType(static_cast<b2BodyType>(type));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		b2Body* Body::GetB2Actor() const
		{
			return _b2Body;
		}
	}
}
