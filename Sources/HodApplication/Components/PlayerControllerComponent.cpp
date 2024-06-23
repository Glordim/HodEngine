#include "PlayerControllerComponent.hpp"
#include "BarComponent.hpp"

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Input/InputManager.hpp>

using namespace hod::input;

DESCRIBE_REFLECTED_CLASS(PlayerControllerComponent, ControllerComponent)
{
}

void PlayerControllerComponent::OnAwake()
{
	std::shared_ptr<PawnComponent> pawn = GetEntity()->GetComponent<PawnComponent>();
	if (pawn != nullptr)
	{
		Posses(pawn);
	}
}

void PlayerControllerComponent::OnUpdate()
{
	InputManager* inputManager = InputManager::GetInstance();

	Input::State upState = inputManager->GetInputState(InputId::KeyW);
	if (upState.IsPressed())
	{
		std::static_pointer_cast<BarComponent>(GetPawn())->Move(upState._value);
	}

	Input::State downState = inputManager->GetInputState(InputId::KeyS);
	if (downState.IsPressed())
	{
		std::static_pointer_cast<BarComponent>(GetPawn())->Move(-downState._value);
	}
}
