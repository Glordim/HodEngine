#pragma once

#include "ControllerComponent.hpp"

class PlayerControllerComponent : public ControllerComponent
{
	REFLECTED_CLASS(PlayerControllerComponent, ControllerComponent);

public:

	void OnAwake() override;
	void OnUpdate() override;
};
