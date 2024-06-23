#pragma once

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/WeakComponent.hpp>

class PawnComponent;

class ControllerComponent : public hod::game::Component
{
	REFLECTED_CLASS(ControllerComponent, hod::game::Component);

public:

	void Posses(std::shared_ptr<PawnComponent> pawn);
	void Unposses();

	std::shared_ptr<PawnComponent> GetPawn() const;

private:

	std::weak_ptr<PawnComponent>	_pawn;
};
