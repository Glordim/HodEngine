#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/World.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::physics
{
	/// @brief 
	class HOD_PHYSICS_API WorldBox2d : public World
	{
	public:
				WorldBox2d();
				~WorldBox2d() override;

	public:

		Body*				CreateBody(Body::Type type, const Vector2& position, float rotation) override;
		void				DeleteBody(Body* body) override;

		void				Update(float dt) override;

		bool				Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result) override;

		ColliderBox2d*		FindColliderByB2ShapeId(b2ShapeId shapeId) const;

		b2WorldId			GetWorldId() const;

	private:

		b2WorldId			_worldId = {};
	};
}
