#include "HodEngine/Physics/Pch.hpp"
#include "BodyBox2d.hpp"

#include "Physics.hpp"

#include <box2d/box2d.h>

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
		BodyBox2d::BodyBox2d(b2BodyId b2BodyId)
		: _b2BodyId(b2BodyId)
		{
		}

		/// @brief 
		BodyBox2d::~BodyBox2d()
		{
			for (b2ShapeId shapeId : _shapeIds)
			{
				b2DestroyShape(shapeId);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition)
		{
			b2Segment segment;
			segment.point1 = { startPosition.GetX(), startPosition.GetY() };
			segment.point2 = { endPosition.GetX(), endPosition.GetY() };

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2ShapeId shapeId = b2CreateSegmentShape(_b2BodyId, &shapeDef, &segment);
			_shapeIds.push_back(shapeId);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::AddCircleShape(const Vector2& position, float radius)
		{
			b2Circle circle;
			circle.center = { position.GetX(), position.GetY() };
			circle.radius = radius;

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2ShapeId shapeId = b2CreateCircleShape(_b2BodyId, &shapeDef, &circle);
			_shapeIds.push_back(shapeId);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density)
		{
			b2Polygon polygon = b2MakeBox(size.GetX(), size.GetY());
			polygon.centroid = { position.GetX(), position.GetY() };

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2ShapeId shapeId = b2CreatePolygonShape(_b2BodyId, &shapeDef, &polygon);
			_shapeIds.push_back(shapeId);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::AddConvexShape(const std::vector<const Vector2>& vertices)
		{
			//b2PolygonShape shape;
			// todo
			//shape.Set(vertices.data(), vertices.size());

			//_b2Body->CreateFixture(&shape, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::SetTransform(const Vector2& position, float rotation, const Vector2& scale)
		{
			// todo scale
			b2Body_SetTransform(_b2BodyId, { position.GetX(), position.GetY() }, b2MakeRot(math::DegreeToRadian(rotation)));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Vector2 BodyBox2d::GetPosition() const
		{
			b2Vec2 position = b2Body_GetPosition(_b2BodyId);
			return Vector2(position.x, position.y);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float BodyBox2d::GetRotation() const
		{
			b2Rot rotation = b2Body_GetRotation(_b2BodyId);
			return b2Rot_GetAngle(rotation);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Body::Type BodyBox2d::GetType() const
		{
			return static_cast<Type>(b2Body_GetType(_b2BodyId));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BodyBox2d::SetType(Type type)
		{
			b2Body_SetType(_b2BodyId, static_cast<b2BodyType>(type));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		b2BodyId BodyBox2d::GetB2Actor() const
		{
			return _b2BodyId;
		}
	}
}
