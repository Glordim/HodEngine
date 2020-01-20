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
        this->initialPos = this->sceneComponent->getPosition();
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    void FlyingPointLight::update(float dt)
    {
        Uint32 ticks = SDL_GetTicks();
        float time = ticks / 1000.0f;

        this->sceneComponent->setPosition(this->initialPos + glm::vec3(0.0f, 5.0f, 0.0f) * sin((float)time * 2));
    }
}
