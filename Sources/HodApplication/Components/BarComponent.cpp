#include "BarComponent.hpp"
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

using namespace hod;

DESCRIBE_REFLECTED_CLASS(BarComponent, PawnComponent)
{
	ADD_PROPERTY(BarComponent, _speed);
}

void BarComponent::OnAwake()
{
	_node = GetEntity()->GetComponent<hod::game::Node2dComponent>();
}

void BarComponent::Move(float value)
{
	_node.lock()->SetPosition(_node.lock()->GetPosition() + Vector2(0.0f, value * _speed * 0.016f)); // TODO use SystemTime
}
