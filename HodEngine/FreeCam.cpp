#include "FreeCam.h"

#include "SceneComponent.h"
#include "CameraComponent.h"

#include "GLFW/glfw3.h"

#include "InputListener.h"

#include "AntTweakBar.h"

FreeCam::FreeCam(const std::string& name, Scene* scene) : Actor(name, scene)
{
    this->sceneComponent = this->addComponent<SceneComponent>();
    this->cameraComponent = this->addComponent<CameraComponent>();

    this->view = glm::vec3(0.0f, 0.0f, 0.0f);

    this->allowRotation = false;

    TwBar* myBar;

    myBar = TwNewBar("CameraTweakBar");

    this->sceneComponent->setupTweakBar(myBar);
}

FreeCam::~FreeCam()
{
}

void FreeCam::setupInputListener(InputListener* inputListener)
{
    inputListener->registerAxisEvent(InputListener::KeyAxis(GLFW_KEY_S, GLFW_KEY_W), std::bind(&FreeCam::moveForward, this, std::placeholders::_1));
    inputListener->registerAxisEvent(InputListener::KeyAxis(GLFW_KEY_A, GLFW_KEY_D), std::bind(&FreeCam::moveRight, this, std::placeholders::_1));
    inputListener->registerAxisEvent(InputListener::KeyAxis(GLFW_KEY_C, GLFW_KEY_SPACE), std::bind(&FreeCam::moveUp, this, std::placeholders::_1));

    inputListener->registerMouseButtonEvent(GLFW_MOUSE_BUTTON_1, GLFW_PRESS, std::bind(&FreeCam::selectObject, this));

    inputListener->registerMouseButtonEvent(GLFW_MOUSE_BUTTON_2, GLFW_PRESS, std::bind(&FreeCam::allowRotate, this));
    inputListener->registerMouseButtonEvent(GLFW_MOUSE_BUTTON_2, GLFW_RELEASE, std::bind(&FreeCam::disallowRotate, this));
    inputListener->registerMouseMoveEvent(std::bind(&FreeCam::rotateView, this, std::placeholders::_1, std::placeholders::_2));
}

void FreeCam::selectObject()
{
    glm::mat4 screenToWorldMat = glm::inverse(this->cameraComponent->getProjectionMatrix());// *this->sceneComponent->getModelMatrix());

    glm::vec4 dir;
    dir.x = this->mouseX / 1920.0f;
    dir.y = this->mouseY / 1080.0f;
    dir.z = 1.0f;
    dir.w = 1.0f;

    dir = dir * screenToWorldMat;

    dir.w = 1.0 / dir.w;

    dir.x *= dir.w;
    dir.y *= dir.w;
    dir.z *= dir.w;

    physx::PxRaycastBuffer result;

    if (this->scene->raycast(this->sceneComponent->getPosition(), dir, 100.0f, result) == true)
    {
        physx::PxShape* pxShape = result.touches[0].shape;
        physx::PxActor* pxActor = pxShape->getActor();

        Actor* actor = this->scene->convertPxActor(pxActor);
        if (actor != nullptr)
        {

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
    this->mouseX = x;
    this->mouseY = y;

    if (this->allowRotation == false)
        return;

    //this->sceneComponent->rotate(((float)x - (1920.0f * 0.5f)) / (1920.0f * 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    //this->sceneComponent->rotate(((float)y - (1080.0f * 0.5f)) / (1080.0f * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f));

    float angleX = ((float)x - (1920.0f * 0.5f)) / (1920.0f * 0.5f);
    float angleY = ((float)y - (1080.0f * 0.5f)) / (1080.0f * 0.5f);

    this->view.x += angleX;
    this->view.y += angleY;

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
    this->movement.z = axisValue;
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

