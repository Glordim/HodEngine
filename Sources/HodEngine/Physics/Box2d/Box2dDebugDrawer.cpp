#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/Box2dDebugDrawer.hpp"
#include "HodEngine/Physics/Box2d/Box2dPhysics.hpp"
#include "HodEngine/Physics/Box2d/Box2dWorld.hpp"

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"

#include <box2d/box2d.h>

namespace hod::inline physics
{
	/// @brief
	Box2dDebugDrawer::Box2dDebugDrawer()
	{
		_debugDraw.drawingBounds = {};
		_debugDraw.useDrawingBounds = false;

		_debugDraw.drawShapes = true;
		_debugDraw.drawJoints = false;
		_debugDraw.drawJointExtras = false;
		_debugDraw.drawAABBs = false;
		_debugDraw.drawMass = false;
		_debugDraw.drawContacts = false;
		_debugDraw.drawGraphColors = false;
		_debugDraw.drawContactNormals = false;
		_debugDraw.drawContactImpulses = false;
		_debugDraw.drawFrictionImpulses = true;
		_flags = (1 << 0); // Shapes

		_debugDraw.context = this;

		_debugDraw.DrawPolygon = &Box2dDebugDrawer::DrawPolygon;
		_debugDraw.DrawSolidPolygon = &Box2dDebugDrawer::DrawSolidPolygon;
		_debugDraw.DrawCircle = &Box2dDebugDrawer::DrawCircle;
		_debugDraw.DrawSolidCircle = &Box2dDebugDrawer::DrawSolidCircle;
		_debugDraw.DrawSolidCapsule = &Box2dDebugDrawer::DrawSolidCapsule;
		_debugDraw.DrawSegment = &Box2dDebugDrawer::DrawSegment;
		_debugDraw.DrawTransform = &Box2dDebugDrawer::DrawTransform;
		_debugDraw.DrawPoint = &Box2dDebugDrawer::DrawPoint;
		_debugDraw.DrawString = &Box2dDebugDrawer::DrawString;
	}

	/// @brief
	/// @return
	const Vector<DebugDrawer::Flag>& Box2dDebugDrawer::GetAvailableFlags() const
	{
		static Vector<Flag> flags = {
			Flag("Shapes", 1 << 0),   Flag("Joints", 1 << 1),      Flag("JointExtras", 1 << 2),    Flag("AABBs", 1 << 3),           Flag("Mass", 1 << 4),
			Flag("Contacts", 1 << 5), Flag("GraphColors", 1 << 6), Flag("ContactNormals", 1 << 7), Flag("ContactImpulses", 1 << 8), Flag("FrictionImpulses", 1 << 9),
		};

		return flags;
	}

	/// @brief
	/// @param flags
	void Box2dDebugDrawer::SetFlags(uint32_t flags)
	{
		_flags = flags;

		_debugDraw.drawShapes = (_flags & (1 << 0));
		_debugDraw.drawJoints = (_flags & (1 << 1));
		_debugDraw.drawJointExtras = (_flags & (1 << 2));
		_debugDraw.drawAABBs = (_flags & (1 << 3));
		_debugDraw.drawMass = (_flags & (1 << 4));
		_debugDraw.drawContacts = (_flags & (1 << 5));
		_debugDraw.drawGraphColors = (_flags & (1 << 6));
		_debugDraw.drawContactNormals = (_flags & (1 << 7));
		_debugDraw.drawContactImpulses = (_flags & (1 << 8));
		_debugDraw.drawFrictionImpulses = (_flags & (1 << 9));
	}

	/// @brief
	/// @return
	uint32_t Box2dDebugDrawer::GetFlags() const
	{
		return _flags;
	}

	/// @brief
	void Box2dDebugDrawer::Draw(World* world, DebugDrawTarget& target)
	{
		_target = &target;
		b2World_Draw(static_cast<Box2dWorld*>(world)->GetWorldId(), &_debugDraw);
		_target = nullptr;
	}

	/// @brief
	Color Box2dDebugDrawer::ToColor(b2HexColor color)
	{
		return Color((int32_t)(((uint32_t)color << 8) | 0xff));
	}

	/// @brief Darker: the fill of a solid shape, its outline in color drawn over it
	Color Box2dDebugDrawer::ToFillColor(b2HexColor color)
	{
		Color fillColor = ToColor(color);
		fillColor[0] *= 0.5f;
		fillColor[1] *= 0.5f;
		fillColor[2] *= 0.5f;
		return fillColor;
	}

	/// @brief
	void Box2dDebugDrawer::DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		static_assert(sizeof(b2Vec2) == sizeof(Vector2));
		thiz->_target->Polygon(reinterpret_cast<const Vector2*>(vertices), (uint32_t)vertexCount, ToColor(color), false);
	}

	/// @brief
	void Box2dDebugDrawer::DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context)
	{
		(void)radius; // TODO rounded polygon

		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		Vector<Vector2> points;
		points.Reserve((uint32_t)vertexCount);
		for (int index = 0; index < vertexCount; ++index)
		{
			b2Vec2 point = b2TransformPoint(transform, vertices[index]);
			points.PushBack(Vector2(point.x, point.y));
		}

		thiz->_target->Polygon(points.Data(), (uint32_t)points.Size(), ToFillColor(color), true);
		thiz->_target->Polygon(points.Data(), (uint32_t)points.Size(), ToColor(color), false);
	}

	/// @brief
	void Box2dDebugDrawer::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		thiz->_target->Circle(Vector2(center.x, center.y), radius, ToColor(color), false);
	}

	/// @brief
	void Box2dDebugDrawer::DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		Vector2 center(transform.p.x, transform.p.y);
		thiz->_target->Circle(center, radius, ToFillColor(color), true);
		thiz->_target->Circle(center, radius, ToColor(color), false);
		thiz->_target->Segment(center, center + Vector2(transform.q.c, transform.q.s) * radius, ToColor(color)); // its rotation
	}

	/// @brief
	void Box2dDebugDrawer::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		Vector2 start(p1.x, p1.y);
		Vector2 end(p2.x, p2.y);
		thiz->_target->Capsule(start, end, radius, ToFillColor(color), true);
		thiz->_target->Capsule(start, end, radius, ToColor(color), false);
	}

	/// @brief
	void Box2dDebugDrawer::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		thiz->_target->Segment(Vector2(p1.x, p1.y), Vector2(p2.x, p2.y), ToColor(color));
	}

	/// @brief Its axes: x in red, y in green
	void Box2dDebugDrawer::DrawTransform(b2Transform transform, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		constexpr float axisLength = 0.2f;
		Vector2         origin(transform.p.x, transform.p.y);
		thiz->_target->Segment(origin, origin + Vector2(transform.q.c, transform.q.s) * axisLength, Color(1.0f, 0.0f, 0.0f, 1.0f));
		thiz->_target->Segment(origin, origin + Vector2(-transform.q.s, transform.q.c) * axisLength, Color(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief
	void Box2dDebugDrawer::DrawPoint(b2Vec2 p, float size, b2HexColor color, void* context)
	{
		Box2dDebugDrawer* thiz = static_cast<Box2dDebugDrawer*>(context);

		thiz->_target->Point(Vector2(p.x, p.y), size, ToColor(color));
	}

	/// @brief
	void Box2dDebugDrawer::DrawString(b2Vec2 p, const char* s, b2HexColor color, void* context)
	{
		// TODO
		(void)p;
		(void)s;
		(void)color;
		(void)context;
	}
}
