#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Weakable/WeakableAliveController.hpp"

namespace hod::inline core
{
	void WeakableAliveController::MarkAsDead()
	{
		_alive = false;
	}

	bool WeakableAliveController::IsAlive() const
	{
		return _alive;
	}
}
