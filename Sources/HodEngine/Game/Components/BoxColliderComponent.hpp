#pragma once

#include "HodEngine/Game/Components/ColliderComponent.hpp"

#include <vector>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class BoxColliderComponent : public ColliderComponent
		{
			REFLECTED_CLASS(BoxColliderComponent, ColliderComponent)

		public:

							BoxColliderComponent() = default;
							BoxColliderComponent(const BoxColliderComponent&) = delete;
							BoxColliderComponent(BoxColliderComponent&&) = delete;
							~BoxColliderComponent() override = default;

			void			operator=(const BoxColliderComponent&) = delete;
			void			operator=(BoxColliderComponent&&) = delete;
		};
	}
}
