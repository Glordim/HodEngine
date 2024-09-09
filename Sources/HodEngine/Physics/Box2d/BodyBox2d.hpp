#pragma once
#include "HodEngine/Physics/Export.hpp"
#include "HodEngine/Physics/Body.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::physics
{
	/// @brief 
	class HOD_PHYSICS_API BodyBox2d : public Body
	{
	public:
							BodyBox2d(b2BodyId b2BodyId);
							~BodyBox2d() override = default;

	public:

		Collider*			AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition) override;
		Collider*			AddCircleShape(const Vector2& position, float radius) override;
		Collider*			AddCapsuleShape(const Vector2& position, float height, float radius, float angle) override;
		Collider*			AddBoxShape(const Vector2& position, const Vector2& size, float angle) override;
		Collider*			AddConvexShape(const std::vector<const Vector2>& vertices) override;

		void				SetTransform(const Vector2& position, float angle, const Vector2& scale) override;

		Vector2				GetPosition() const override;
		float				GetRotation() const override;

		Type				GetType() const override;
		void				SetType(Type type) override;

		b2BodyId			GetB2Actor() const;

	private:

		b2BodyId			_b2BodyId = b2_nullBodyId;
	};
}
