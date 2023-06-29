#pragma once

#include "HodEngine/Game/src/Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/glm.hpp>

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

							ColliderComponent(const std::weak_ptr<Entity>& entity);
							ColliderComponent(const ColliderComponent&) = delete;
							ColliderComponent(ColliderComponent&&) = delete;
							~ColliderComponent() override = default;

			void			operator=(const ColliderComponent&) = delete;
			void			operator=(ColliderComponent&&) = delete;

		public:

			const char*		GetType() const override;

			void			AddEdgeShape(const glm::vec2& startPosition, const glm::vec2& endPosition);
			void			AddCircleShape(const glm::vec2& position, float radius);
			void			AddBoxShape(const glm::vec2& position, const glm::vec2& size, float angle, float density);
			void			AddConvexShape(const std::vector<const glm::vec2>& vertices);
		};
	}
}
