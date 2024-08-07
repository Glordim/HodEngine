#pragma once
#include <HodEngine/HodEngine.hpp>

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
		class HOD_API RigidbodyComponent : public Component
		{
			REFLECTED_CLASS(RigidbodyComponent, Component)

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
