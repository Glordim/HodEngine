#include "ControllerComponent.hpp"
#include "PawnComponent.hpp"

DESCRIBE_REFLECTED_CLASS(ControllerComponent, hod::game::Component)
{
}

void ControllerComponent::Posses(std::shared_ptr<PawnComponent> pawn)
{
	std::shared_ptr<PawnComponent> previousPawn = _pawn.lock();
	if (previousPawn != nullptr)
	{
		Unposses();
	}

	_pawn = pawn;
	pawn->SetController(std::static_pointer_cast<ControllerComponent>(shared_from_this()));
}

void ControllerComponent::Unposses()
{
	std::shared_ptr<PawnComponent> previousPawn = _pawn.lock();
	if (previousPawn != nullptr)
	{
		previousPawn->SetController(nullptr);
		_pawn.reset();
	}
}

std::shared_ptr<PawnComponent> ControllerComponent::GetPawn() const
{
	return _pawn.lock();
}
