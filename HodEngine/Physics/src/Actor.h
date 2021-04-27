#pragma once

#include <vector>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/glm.hpp>

class b2Body;

namespace HOD
{
	namespace RENDERER
	{
		struct BoundingBox;
	}

	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Actor
		{
		public:
								Actor(b2Body* b2Body);
								~Actor() = default;

		public:

			void				AddEdgeShape(const glm::vec2& startPosition, const glm::vec2& endPosition);
			void				AddCircleShape(const glm::vec2& position, float radius);
			void				AddBoxShape(const glm::vec2& position, const glm::vec2& size, float angle);
			void				AddConvexShape(const std::vector<const glm::vec2>& vertices);

			void				SetTransform(const glm::vec2& position, float angle, const glm::vec2& scale);

			b2Body*				GetB2Actor() const;

		private:

			b2Body*				_b2Body = nullptr;
		};
	}
}
