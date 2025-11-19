#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

#include <array>

namespace hod::editor
{
	/// @brief 
	class GeometryGenerator
	{
	public:

		template<uint32_t SegmentCount_>
		static void CircleShape(std::array<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CircleShapeFillNoFan(std::array<Vector2, (SegmentCount_) * 3>& vertices, const Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CapsuleShape(std::array<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float height, float radius);
	};
}

#include "GeometryGenerator.inl"
