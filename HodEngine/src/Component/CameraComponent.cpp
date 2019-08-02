#include "CameraComponent.h"

#include "glm/gtc/matrix_transform.hpp"

#include "SceneComponent.h"
#include "StaticMeshComponent.h"
#include "LightComponent.h"

#include "AntTweakBar.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/RenderQueue.h"
#include "../RenderQueueHelper.h"

CameraComponent::CameraComponent(Actor* actor)
: Component(actor)
, fov(60.0f)
, aspect(1920.0f / 1080.0f)
, fNear(0.01f)
, fFar(1000.0f)
, perspective(true)
, dirtyFlag(true)
{

}

CameraComponent::~CameraComponent()
{

}

void CameraComponent::setupTweakBar(TwBar* tweakBar)
{
    TwAddSeparator(tweakBar, "Camera", "");
    TwAddVarRW(tweakBar, "fov", TW_TYPE_FLOAT, &this->fov, "");
    TwAddVarRW(tweakBar, "near", TW_TYPE_FLOAT, &this->fNear, "");
    TwAddVarRW(tweakBar, "far", TW_TYPE_FLOAT, &this->fFar, "");
}

const glm::mat4& CameraComponent::getProjectionMatrix()
{
    if (this->dirtyFlag == true)
    {
        if (this->perspective == true)
            this->projectionMatrix = glm::perspective(glm::radians(this->fov), this->aspect, this->fNear, this->fFar);
        else
            this->projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, this->fNear, this->fFar);

        this->dirtyFlag = false;
    }

    return this->projectionMatrix;
}

void CameraComponent::render(Scene& scene)
{
    RenderQueue renderQueue;

    renderQueue.SetViewMatrix(glm::inverse(this->getActor()->getComponent<SceneComponent>()->getModelMatrix()));
    renderQueue.SetProjMatrix(this->getProjectionMatrix());

    renderQueue.SetClearFlag(RenderQueue::ClearFlag::COLOR | RenderQueue::ClearFlag::DEPTH);

    RenderQueueHelper::AddSceneComponent(renderQueue, scene.getRoot(), true);
    RenderQueueHelper::AddScenePhysicsDebug(renderQueue, &scene);

    Renderer* renderer = Renderer::GetInstance();
    renderer->SubmitRenderQueue(renderQueue);
}

void CameraComponent::drawSceneComponent(SceneComponent* sceneComponent, std::vector<LightComponent*>& allLight)
{
    size_t childCount = sceneComponent->getChildCount();
    for (int i = 0; i < childCount; ++i)
    {
        SceneComponent* child = sceneComponent->getChild(i);

        this->drawSceneComponent(child, allLight);

        StaticMeshComponent* staticMeshComponent = child->getActor()->getComponent<StaticMeshComponent>();
        if (staticMeshComponent != nullptr)
        {
            staticMeshComponent->draw(this, allLight);
        }
    }
}
