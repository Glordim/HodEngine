#pragma once
#include "HodEngine/Editor/Export.hpp"
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Core/StaticArray.hpp"

namespace hod::inline editor
{
	/// @brief 
	class GeometryGenerator
	{
	public:

		template<uint32_t SegmentCount_>
		static void CircleShape(StaticArray<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CircleShapeFillNoFan(StaticArray<Vector2, (SegmentCount_) * 3>& vertices, const Vector2& center, float radius);

		template<uint32_t SegmentCount_>
		static void CapsuleShape(StaticArray<Vector2, SegmentCount_ + 1>& vertices, const Vector2& center, float height, float radius);
	};
}

#include "GeometryGenerator.inl"
