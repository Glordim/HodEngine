#pragma once

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

class AutoRotateComponent : public hod::game::Component
{
	REFLECTED_CLASS(AutoRotateComponent, hod::game::Component);

public:

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:

	std::weak_ptr<hod::game::Node2dComponent>	_node;

	float										_speed = 20.0f;
};
