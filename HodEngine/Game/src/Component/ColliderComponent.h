#pragma once

#include "../Component.h"

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

			void			SetShape(PHYSICS::SHAPE eShape);
		};
	}
}
