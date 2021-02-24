#include "FlyingPointLight.h"

#include "HodEngine/Game/src/Component/SceneComponent.h"

#include <SDK/SDL/include/SDL.h>

namespace HOD
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	FlyingPointLight::FlyingPointLight(const std::string& name, GAME::Scene* scene) : GAME::Actor(name, scene)
	{
		_sceneComponent = AddComponent<GAME::SceneComponent>();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	FlyingPointLight::~FlyingPointLight()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void FlyingPointLight::Start()
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void FlyingPointLight::Update(float dt)
	{
		Uint32 ticks = SDL_GetTicks();
		float time = ticks / 1000.0f;

		_sceneComponent->SetPosition(_sceneComponent->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * sin((float)time) * dt);
	}
}
