#include "FreeCam.h"

#include "../Component/SceneComponent.h"
#include "../Component/CameraComponent.h"

#include "../InputListener.h"

#include "AntTweakBar.h"

#include <SDL.h>

FreeCam::FreeCam(const std::string& name, Scene* scene) : Actor(name, scene)
{
    this->sceneComponent = this->addComponent<SceneComponent>();
    this->cameraComponent = this->addComponent<CameraComponent>();

    this->view = glm::vec3(0.0f, 0.0f, 0.0f);

    this->allowRotation = false;

    this->myBar = TwNewBar("Actor Picker");
    this->setupTweakBarForAllComponent(this->myBar);
}

FreeCam::~FreeCam()
{
    TwDeleteBar(this->myBar);
}

void FreeCam::setupInputListener(InputListener* inputListener)
{
    inputListener->registerAxisEvent(InputListener::KeyAxis(SDLK_s, SDLK_w), std::bind(&FreeCam::moveForward, this, std::placeholders::_1));
    inputListener->registerAxisEvent(InputListener::KeyAxis(SDLK_a, SDLK_d), std::bind(&FreeCam::moveRight, this, std::placeholders::_1));
    inputListener->registerAxisEvent(InputListener::KeyAxis(SDLK_c, SDLK_SPACE), std::bind(&FreeCam::moveUp, this, std::placeholders::_1));

    inputListener->registerMouseButtonEvent(SDL_BUTTON_LEFT, SDL_PRESSED, std::bind(&FreeCam::selectObject, this));

    inputListener->registerMouseButtonEvent(SDL_BUTTON_RIGHT, SDL_PRESSED, std::bind(&FreeCam::allowRotate, this));
    inputListener->registerMouseButtonEvent(SDL_BUTTON_RIGHT, SDL_RELEASED, std::bind(&FreeCam::disallowRotate, this));
    inputListener->registerMouseMoveEvent(std::bind(&FreeCam::rotateView, this, std::placeholders::_1, std::placeholders::_2));
}

void FreeCam::selectObject()
{
    float mouseZ = 1.0f;

    glm::mat4 screenToWorldMat = this->cameraComponent->getProjectionMatrix() * glm::inverse(this->sceneComponent->getModelMatrix());
    screenToWorldMat = glm::inverse(screenToWorldMat);

    glm::vec4 dir;
    dir.x = (this->mouseX - 0.0f) / 1920.0f * 2.0f - 1.0f;
    dir.y = ((1080.0f - this->mouseY - 0.0f)) / 1080.0f * 2.0f - 1.0f;
    dir.z = 2.0f * mouseZ - 1.0f;
    dir.w = 1.0f;

    dir = screenToWorldMat * dir;

    dir.w = 1.0f / dir.w;

    dir.x *= dir.w;
    dir.y *= dir.w;
    dir.z *= dir.w;

    dir = glm::normalize(dir);

    glm::vec3 finalPos = this->sceneComponent->getPosition() + glm::vec3(dir * 100.0f);

    physx::PxRaycastBuffer result;

    if (this->scene->raycast(this->sceneComponent->getPosition(), dir, 100.0f, result, true, Color(1.0f, 1.0f, 0.0f, 1.0f), 5.0f) == true)
    {
        physx::PxActor* pxActor = result.block.actor;

        Actor* actor = this->scene->convertPxActor(pxActor);
        if (actor != nullptr)
        {
            actor->setupTweakBarForAllComponent(this->myBar);
        }
    }
}

void FreeCam::allowRotate()
{
    this->allowRotation = true;
}

void FreeCam::disallowRotate()
{
    this->allowRotation = false;
}

void FreeCam::rotateView(int x, int y)
{
    this->mouseX = (float)x;
    this->mouseY = (float)y;

    if (this->allowRotation == false)
        return;

    //this->sceneComponent->rotate(((float)x - (1920.0f * 0.5f)) / (1920.0f * 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    //this->sceneComponent->rotate(((float)y - (1080.0f * 0.5f)) / (1080.0f * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f));

    float angleX = ((float)x - (1920.0f * 0.5f)) / (1920.0f * 0.5f);
    float angleY = ((float)y - (1080.0f * 0.5f)) / (1080.0f * 0.5f);

    this->view.x += -angleX;
    this->view.y += -angleY;

    this->sceneComponent->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    this->sceneComponent->rotate(view.x, glm::vec3(0.0f, 1.0f, 0.0f));
    this->sceneComponent->rotate(view.y, glm::vec3(1.0f, 0.0f, 0.0f));

    //this->sceneComponent->setPosition()
    /*
    glm::quat rot = glm::quat(view);

    this->sceneComponent->setRotation(view);
    */
}

void FreeCam::moveForward(float axisValue)
{
    this->movement.z = -axisValue;
}

void FreeCam::moveRight(float axisValue)
{
    this->movement.x = axisValue;
}

void FreeCam::moveUp(float axisValue)
{
    this->movement.y = axisValue;
}

void FreeCam::update(float dt)
{
    //this->sceneComponent->setRotation(glm::degrees(glm::eulerAngles(this->tmpQuat)));
    this->sceneComponent->setPosition(sceneComponent->getPosition() + (sceneComponent->getRotation() * this->movement * dt * 5.0f));
}

