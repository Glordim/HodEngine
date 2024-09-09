#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

#include <vector>

namespace hod::physics
{
	class Body;
}

namespace hod::game
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API Rigidbody2dComponent : public Component
	{
		REFLECTED_CLASS(Rigidbody2dComponent, Component, HOD_GAME_API)

	public:

						Rigidbody2dComponent() = default;
						Rigidbody2dComponent(const Rigidbody2dComponent&) = delete;
						Rigidbody2dComponent(Rigidbody2dComponent&&) = delete;
						~Rigidbody2dComponent() override;

		void			operator=(const Rigidbody2dComponent&) = delete;
		void			operator=(Rigidbody2dComponent&&) = delete;

	public:

		void			OnAwake() override;
		void			OnStart() override;
		void			OnUpdate() override;

		physics::Body*	GetInternalBody() const;

		void			SetDynamic(bool dynamic);
		bool			IsDynamic() const;

	private:

		physics::Body*	_body = nullptr;
		
		bool			_dynamic = false;
	};
}
