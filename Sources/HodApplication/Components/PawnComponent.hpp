#pragma once

#include <HodEngine/Game/Component.hpp>

class ControllerComponent;

class PawnComponent : public hod::game::Component
{
	REFLECTED_CLASS(PawnComponent, hod::game::Component);

	friend class ControllerComponent;

public:

	std::shared_ptr<ControllerComponent> GetController() const;

private:

	void SetController(std::shared_ptr<ControllerComponent> controller);

private:

	std::weak_ptr<ControllerComponent> _controller;
};
