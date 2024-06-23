#include "PawnComponent.hpp"

DESCRIBE_REFLECTED_CLASS(PawnComponent, hod::game::Component)
{
}

std::shared_ptr<ControllerComponent> PawnComponent::GetController() const
{
	return _controller.lock();
}

void PawnComponent::SetController(std::shared_ptr<ControllerComponent> controller)
{
	_controller = controller;
}
