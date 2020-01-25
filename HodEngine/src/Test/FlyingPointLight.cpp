#include "FlyingPointLight.h"

#include "../Game/Component/SceneComponent.h"

#include <SDL.h>

namespace HOD
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    FlyingPointLight::FlyingPointLight(const std::string& name, GAME::Scene* scene) : GAME::Actor(name, scene)
    {
        this->sceneComponent = this->addComponent<GAME::SceneComponent>();
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
    void FlyingPointLight::start()
    {
        
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    void FlyingPointLight::update(float dt)
    {
        Uint32 ticks = SDL_GetTicks();
        float time = ticks / 1000.0f;

        this->sceneComponent->setPosition(this->sceneComponent->getPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * sin((float)time) * dt);
    }
}
