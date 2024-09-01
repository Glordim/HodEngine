#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Box2d/DebugDrawerBox2d.hpp"
#include "HodEngine/Physics/Box2d/PhysicsBox2d.hpp"

#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Math.hpp"

#include <box2d/box2d.h>

namespace hod::physics
{
	/// @brief 
	DebugDrawerBox2d::DebugDrawerBox2d()
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

		_debugDraw.DrawPolygon = &DebugDrawerBox2d::DrawPolygon;
		_debugDraw.DrawSolidPolygon = &DebugDrawerBox2d::DrawSolidPolygon;
		_debugDraw.DrawCircle = &DebugDrawerBox2d::DrawCircle;
		_debugDraw.DrawSolidCircle = &DebugDrawerBox2d::DrawSolidCircle;
		_debugDraw.DrawCapsule = &DebugDrawerBox2d::DrawCapsule;
		_debugDraw.DrawSolidCapsule = &DebugDrawerBox2d::DrawSolidCapsule;
		_debugDraw.DrawSegment = &DebugDrawerBox2d::DrawSegment;
		_debugDraw.DrawTransform = &DebugDrawerBox2d::DrawTransform;
		_debugDraw.DrawPoint = &DebugDrawerBox2d::DrawPoint;
		_debugDraw.DrawString = &DebugDrawerBox2d::DrawString;
	}

	/// @brief 
	/// @return 
	const std::vector<DebugDrawer::Flag>& DebugDrawerBox2d::GetAvailableFlags() const
	{
		static std::vector<Flag> flags = {
			Flag("Shapes", (1 << 0)),
			Flag("Joints", (1 << 1)),
			Flag("JointExtras", (1 << 2)),
			Flag("AABBs", (1 << 3)),
			Flag("Mass", (1 << 4)),
			Flag("Contacts", (1 << 5)),
			Flag("GraphColors", (1 << 6)),
			Flag("ContactNormals", (1 << 7)),
			Flag("ContactImpulses", (1 << 8)),
			Flag("FrictionImpulses", (1 << 9)),
		};

		return flags;
	}

	/// @brief 
	/// @param flags 
	void  DebugDrawerBox2d::SetFlags(uint32_t flags)
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
	uint32_t DebugDrawerBox2d::GetFlags() const
	{
		return _flags;
	}
	
	/// @brief 
	void DebugDrawerBox2d::Update()
	{
		_renderCommands.clear();

		b2World_Draw(PhysicsBox2d::GetInstance()->GetWorldId(), &_debugDraw);
	}

	/// @brief 
	/// @param vertices 
	/// @param vertexCount 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::WireframePolygon;
		renderCommand._color = (color << 8) | 0xff;

		renderCommand._vertices.resize(vertexCount * 2);
		std::memcpy(renderCommand._vertices.data(), vertices, sizeof(float) * vertexCount * 2);

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param transform 
	/// @param vertices 
	/// @param vertexCount 
	/// @param radius 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::FillPolygon;
		renderCommand._color = (color << 8) | 0xff;

		// todo use transform ?

		renderCommand._vertices.resize(vertexCount * 2);
		std::memcpy(renderCommand._vertices.data(), vertices, sizeof(float) * vertexCount * 2);

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param center 
	/// @param radius 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::WireframePolygon;
		renderCommand._color = (color << 8) | 0xff;

		uint32_t segmentCount = 32;
		renderCommand._vertices.resize(segmentCount * 6);
		BuildCircleVertices(renderCommand._vertices.data(), center, radius, segmentCount);

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param transform 
	/// @param radius 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::FillPolygon;
		renderCommand._color = (color << 8) | 0xff;

		uint32_t segmentCount = 32;
		renderCommand._vertices.resize(segmentCount * 6);
		BuildCircleVertices(renderCommand._vertices.data(), transform.p, radius, segmentCount); // todo use transform.rot ?

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param vertices 
	/// @param transform 
	/// @param radius 
	/// @param segmentCount 
	void DebugDrawerBox2d::BuildCircleVertices(float* vertices, const b2Vec2& center, float radius, uint32_t segmentCount)
	{
		const float angleStep = 360.0f / segmentCount;

		for (uint32_t currentSegment = 0; currentSegment < segmentCount; ++currentSegment)
		{
			uint32_t offset = currentSegment * 6;

			vertices[offset + 0] = center.x + 0.0f;
			vertices[offset + 1] = center.y + 0.0f;

			float angle = angleStep * currentSegment;
			angle = math::DegreeToRadian(angle);

			vertices[offset + 2] = center.x + (radius * cosf(angle));
			vertices[offset + 3] = center.y + (radius * sinf(angle));

			angle = angleStep * (currentSegment + 1);
			angle = math::DegreeToRadian(angle);

			vertices[offset + 4] = center.x + (radius * cosf(angle));
			vertices[offset + 5] = center.y + (radius * sinf(angle));
		}
	}

	/// @brief 
	/// @param p1 
	/// @param p2 
	/// @param radius 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		// todo
	}

	/// @brief 
	/// @param p1 
	/// @param p2 
	/// @param radius 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		// todo
	}

	/// @brief 
	/// @param p1 
	/// @param p2 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::Line;
		renderCommand._color = (color << 8) | 0xff;
		renderCommand._vertices.resize(2 * 2);
		std::memcpy(renderCommand._vertices.data(), &p1, 2 * sizeof(float));
		std::memcpy(renderCommand._vertices.data() + 2 * sizeof(float), &p2, 2 * sizeof(float));

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param transform 
	/// @param context 
	void DebugDrawerBox2d::DrawTransform(b2Transform transform, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);
	}

	/// @brief 
	/// @param p 
	/// @param size 
	/// @param color 
	/// @param context 
	void DebugDrawerBox2d::DrawPoint(b2Vec2 p, float size, b2HexColor color, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);

		RenderCommand renderCommand;
		renderCommand._type = RenderCommand::Type::Point;
		renderCommand._color = (color << 8) | 0xff;
		renderCommand._vertices.resize(2);
		std::memcpy(renderCommand._vertices.data(), &p, 2 * sizeof(float));

		thiz->_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param p 
	/// @param s 
	/// @param context 
	void DebugDrawerBox2d::DrawString(b2Vec2 p, const char* s, void* context)
	{
		DebugDrawerBox2d* thiz = static_cast<DebugDrawerBox2d*>(context);
	}
}
