#pragma once

#include "HodEngine/Game/Component.h"
#include "HodEngine/Core/Math/Vector2.h"

#include <vector>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ColliderComponent : public Component
		{
		public:

			enum Shape
			{
				Box,
				Sphere,
				Mesh
			};

		public:

							ColliderComponent() = default;
							ColliderComponent(const ColliderComponent&) = delete;
							ColliderComponent(ColliderComponent&&) = delete;
							~ColliderComponent() override = default;

			void			operator=(const ColliderComponent&) = delete;
			void			operator=(ColliderComponent&&) = delete;

		public:

			const char*		GetType() const override;

			void			AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition);
			void			AddCircleShape(const Vector2& position, float radius);
			void			AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density);
			void			AddConvexShape(const std::vector<const Vector2>& vertices);
		};
	}
}
