#include "FlyingPointLight.h"

#include "../Component/SceneComponent.h"

#include <SDL.h>

FlyingPointLight::FlyingPointLight(const std::string& name, Scene* scene) : Actor(name, scene)
{
    this->sceneComponent = this->addComponent<SceneComponent>();
}

FlyingPointLight::~FlyingPointLight()
{
}

void FlyingPointLight::start()
{
    this->initialPos = this->sceneComponent->getPosition();
}

void FlyingPointLight::update(float dt)
{
    Uint32 ticks = SDL_GetTicks();
    float time = ticks / 1000.0f;

    this->sceneComponent->setPosition(this->initialPos + glm::vec3(0.0f, 5.0f, 0.0f) * sin((float)time * 2));
}

