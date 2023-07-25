#pragma once

#include <vector>
#include "HodEngine/Core/Math/Vector2.h"

class b2Body;

namespace hod
{
	namespace renderer
	{
		struct BoundingBox;
	}

	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Actor
		{
		public:

			enum TYPE
			{
				Static,
				Kinematic,
				Dynamic,
			};

		public:
								Actor(b2Body* b2Body);
								~Actor() = default;

		public:

			void				AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition);
			void				AddCircleShape(const Vector2& position, float radius);
			void				AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density);
			void				AddConvexShape(const std::vector<const Vector2>& vertices);

			void				SetTransform(const Vector2& position, float angle, const Vector2& scale);

			Vector2				GetPosition() const;
			float				GetRotation() const;

			TYPE				GetType() const;
			void				SetType(TYPE type);

			b2Body*				GetB2Actor() const;

		private:

			b2Body*				_b2Body = nullptr;
		};
	}
}
