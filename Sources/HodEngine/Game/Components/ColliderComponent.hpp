#pragma once

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <vector>

namespace hod
{
	namespace physics
	{
		class Body;
	}

	namespace game
	{
		struct Shape
		{
			REFLECTED_CLASS_NO_PARENT(Shape)

		public:

			enum Type
			{
				Box,
				Sphere,
				Mesh
			};

			Type _type;			
		};

		struct EdgeShape : public Shape
		{
			REFLECTED_CLASS(EdgeShape, Shape)

		public:

			Vector2 _start;
			Vector2 _end;
		};

		struct CircleShape : public Shape
		{
			REFLECTED_CLASS(CircleShape, Shape)

		public:

			Vector2	_origin;
			float	_radius;
		};

		struct BoxShape : public Shape
		{
			REFLECTED_CLASS(BoxShape, Shape)

		public:

			Vector2	_origin;
			Vector2	_size;
			float	_angle;
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ColliderComponent : public Component
		{
			REFLECTED_CLASS(ColliderComponent, Component)

		public:

							ColliderComponent();
							ColliderComponent(const ColliderComponent&) = delete;
							ColliderComponent(ColliderComponent&&) = delete;
							~ColliderComponent() override;

			void			operator=(const ColliderComponent&) = delete;
			void			operator=(ColliderComponent&&) = delete;

		public:

			void			OnConstruct() override;
			void			OnAwake() override;
			void			OnUpdate() override;

			void			AddEdgeShape(const Vector2& startPosition, const Vector2& endPosition);
			void			AddCircleShape(const Vector2& position, float radius);
			void			AddBoxShape(const Vector2& position, const Vector2& size, float angle, float density);
			void			AddConvexShape(const std::vector<const Vector2>& vertices);

		private:

			physics::Body*	_body;
			std::vector<Shape*>	_shapes;
			bool				_dynamic = false;
		};
	}
}
