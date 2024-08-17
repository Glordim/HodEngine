#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <vector>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API RigidbodyComponent : public Component
		{
			REFLECTED_CLASS(RigidbodyComponent, Component, HOD_GAME_API)

		public:

							RigidbodyComponent() = default;
							RigidbodyComponent(const RigidbodyComponent&) = delete;
							RigidbodyComponent(RigidbodyComponent&&) = delete;
							~RigidbodyComponent() override = default;

			void			operator=(const RigidbodyComponent&) = delete;
			void			operator=(RigidbodyComponent&&) = delete;

		public:


		};
	}
}
