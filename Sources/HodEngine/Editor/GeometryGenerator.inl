#include <HodEngine/Core/Math/Math.hpp>

namespace hod::editor
{
	/// @brief 
	/// @tparam SegmentCount_ 
	/// @param vertices 
	/// @param center 
	/// @param radius 
	template<uint32_t SegmentCount_>
	void GeometryGenerator::CircleShape(std::array<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float radius)
	{
		const float angleStep = 360.0f / SegmentCount_;

		for (uint32_t currentSegment = 0; currentSegment < SegmentCount_; ++currentSegment)
		{
			float angle = angleStep * currentSegment;
			angle = math::DegreeToRadian(angle);

			vertices[currentSegment].SetX(center.GetX() + (radius * cosf(angle)));
			vertices[currentSegment].SetY(center.GetY() + (radius * sinf(angle)));
		}

		vertices[SegmentCount_] = vertices[0];
	}

	/// @brief 
	/// @tparam SegmentCount_ 
	/// @param vertices 
	/// @param center 
	/// @param height 
	/// @param radius 
	template<uint32_t SegmentCount_>
	void GeometryGenerator::CapsuleShape(std::array<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float height, float radius)
	{
		const float angleStep = 360.0f / SegmentCount_;

		Vector2 offset = center + Vector2(0.0f, height * 0.5f - radius);

		for (uint32_t currentSegment = 0; currentSegment < SegmentCount_ / 2; ++currentSegment)
		{
			float angle = angleStep * currentSegment;
			angle = math::DegreeToRadian(angle);

			vertices[currentSegment].SetX(offset.GetX() + (radius * cosf(angle)));
			vertices[currentSegment].SetY(offset.GetY() + (radius * sinf(angle)));
		}

		offset = center - Vector2(0.0f, height * 0.5f - radius);

		for (uint32_t currentSegment = SegmentCount_ / 2; currentSegment < SegmentCount_; ++currentSegment)
		{
			float angle = angleStep * currentSegment;
			angle = math::DegreeToRadian(angle);

			vertices[currentSegment].SetX(offset.GetX() + (radius * cosf(angle)));
			vertices[currentSegment].SetY(offset.GetY() + (radius * sinf(angle)));
		}

		vertices[SegmentCount_] = vertices[0];
	}
}
