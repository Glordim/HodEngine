#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/ColliderComponent.hpp"

#include <vector>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API BoxColliderComponent : public ColliderComponent
		{
			REFLECTED_CLASS(BoxColliderComponent, ColliderComponent, HOD_GAME_API)

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
