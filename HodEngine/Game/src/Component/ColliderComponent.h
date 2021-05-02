#pragma once

#include "../Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/glm.hpp>

namespace HOD
{
	namespace PHYSICS
	{
		enum SHAPE;
	}

	namespace GAME
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

							ColliderComponent(Actor* actor);
							ColliderComponent(const ColliderComponent&) = delete;
							ColliderComponent(ColliderComponent&&) = delete;
							~ColliderComponent() override = default;

			void			operator=(const ColliderComponent&) = delete;
			void			operator=(ColliderComponent&&) = delete;

		public:

			void			DrawImGui() override;
			const char*		GetType() const override;

			void			AddEdgeShape(const glm::vec2& startPosition, const glm::vec2& endPosition);
			void			AddCircleShape(const glm::vec2& position, float radius);
			void			AddBoxShape(const glm::vec2& position, const glm::vec2& size, float angle, float density);
			void			AddConvexShape(const std::vector<const glm::vec2>& vertices);
		};
	}
}
