#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"

namespace hod::inline game
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API AudioSourceComponent : public Component
	{
		REFLECTED_CLASS(AudioSourceComponent, Component)

	public:

							AudioSourceComponent() = default;
							AudioSourceComponent(const AudioSourceComponent&) = delete;
							AudioSourceComponent(AudioSourceComponent&&) = delete;
							~AudioSourceComponent() override = default;

		void				operator=(const AudioSourceComponent&) = delete;
		void				operator=(AudioSourceComponent&&) = delete;
	};
}
