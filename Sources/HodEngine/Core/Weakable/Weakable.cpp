#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Weakable/Weakable.hpp"
#include "HodEngine/Core/Weakable/WeakableAliveController.hpp"

namespace hod
{
	Weakable::Weakable()
	: _aliveController(std::make_shared<WeakableAliveController>())
	{

	}

	Weakable::~Weakable()
	{
		_aliveController->MarkAsDead();
	}

	std::shared_ptr<WeakableAliveController> Weakable::GetAliveController() const
	{
		return _aliveController;
	}
}
