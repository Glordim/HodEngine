#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Math/Vector2.hpp>

#include <array>

namespace hod::editor
{
	/// @brief 
	class GeometryGenerator
	{
	public:

		template<uint32_t SegmentCount_>
		static void CircleShape(std::array<math::Vector2, SegmentCount_ + 1>& vertices, const math::Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CircleShapeFillNoFan(std::array<math::Vector2, (SegmentCount_) * 3>& vertices, const math::Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CapsuleShape(std::array<math::Vector2, SegmentCount_ + 1>& vertices, const math::Vector2& center, float height, float radius);
	};
}

#include "GeometryGenerator.inl"
