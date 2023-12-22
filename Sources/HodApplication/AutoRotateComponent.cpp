#include "AutoRotateComponent.hpp"
#include <HodEngine/Game/Entity.hpp>
//#include <HodEngine/Core/Time/Time.hpp>

DESCRIBE_REFLECTED_CLASS(AutoRotateComponent, hod::game::Component)
{
	ADD_PROPERTY(AutoRotateComponent, _speed);
}

void AutoRotateComponent::OnAwake()
{
	_node = GetEntity().lock()->GetComponent<hod::game::Node2dComponent>();
}

void AutoRotateComponent::OnStart()
{

}

void AutoRotateComponent::OnUpdate()
{
	_node.lock()->SetRotation(_node.lock()->GetRotation() + (_speed * /*Time::GetRootClockDeltaTime()*/ 0.016f));
}
