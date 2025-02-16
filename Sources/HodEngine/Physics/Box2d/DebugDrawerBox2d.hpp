#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/DebugDrawer.hpp"

#include <vector>

#include <box2d/types.h>

namespace hod::physics
{
	/// @brief 
	class HOD_PHYSICS_API DebugDrawerBox2d : public DebugDrawer
	{
	public:
					DebugDrawerBox2d();
					~DebugDrawerBox2d() override = default;

	public:

		void						Update() override;
		const std::vector<Flag>&	GetAvailableFlags() const override;
		void						SetFlags(uint32_t flags) override;
		uint32_t					GetFlags() const override;

	private:

		static void	DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
		static void	DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context);
		static void	DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context);
		static void	DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void* context);
		static void	DrawCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context);
		static void	DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context);
		static void	DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context);
		static void	DrawTransform(b2Transform transform, void* context);
		static void	DrawPoint(b2Vec2 p, float size, b2HexColor color, void* context);
		static void	DrawString(b2Vec2 p, const char* s, b2HexColor color, void* context);

		static void BuildCircleVertices(Vector2* vertices, const b2Vec2& center, float radius, uint32_t segmentCount);

	private:

		b2DebugDraw _debugDraw;

		uint32_t _flags = 0;
	};
}
