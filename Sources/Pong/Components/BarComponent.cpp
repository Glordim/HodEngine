#include "BarComponent.hpp"
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>

using namespace hod;
using namespace hod::game;

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
	std::shared_ptr<Node2dComponent> nodeLock = _node.lock();
	nodeLock->SetPosition(nodeLock->GetPosition() + Vector2(0.0f, value * _speed * 0.016f)); // TODO use SystemTime
}
