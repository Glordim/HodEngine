#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <vector>
#include "HodEngine/Core/Math/Vector2.hpp"

#include <box2d/id.h>

namespace hod::physics
{
	/// @brief 
	class HOD_PHYSICS_API BodyBox2d
	{
	public:

		enum Type
		{
			Static,
			Kinematic,
			Dynamic,
		};

	public:
							BodyBox2d(b2BodyId b2BodyId);
							~BodyBox2d();

	public:

		void				AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition) override;
		void				AddCircleShape(const Vector2& position, float radius) override;
		void				AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density) override;
		void				AddConvexShape(const std::vector<const Vector2>& vertices) override;

		void				SetTransform(const Vector2& position, float angle, const Vector2& scale) override;

		Vector2				GetPosition() const override;
		float				GetRotation() const override;

		Type				GetType() const override;
		void				SetType(Type type) override;

		b2BodyId			GetB2Actor() const;

	private:

		b2BodyId			_b2BodyId = {};

		std::vector<b2ShapeId> _shapeIds;			
	};
}
