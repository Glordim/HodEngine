#include "RenderQueueHelper.h"

#include "RenderQueue.h"
#include "SceneComponent.h"
#include "StaticMeshComponent.h"
#include "LightComponent.h"
#include "Actor.h"


void RenderQueueHelper::AddSceneComponent(RenderQueue& renderQueue, SceneComponent* sceneComponent, bool recursive)
{
    size_t childCount = sceneComponent->getChildCount();
    for (size_t i = 0; i < childCount; ++i)
    {
        SceneComponent* child = sceneComponent->getChild(i);

        if (recursive == true)
            RenderQueueHelper::AddSceneComponent(renderQueue, child, recursive);

        StaticMeshComponent* staticMeshComponent = child->getActor()->getComponent<StaticMeshComponent>();
        if (staticMeshComponent != nullptr)
        {
            renderQueue.AddMesh(staticMeshComponent->GetMesh(), staticMeshComponent->GetMaterial(), child->getModelMatrix());
        }

        LightComponent* lightComponent = child->getActor()->getComponent<LightComponent>();
        if (lightComponent != nullptr)
        {
            renderQueue.AddPointLight(lightComponent->GetPointLight(), child->getModelMatrix());
        }
    }
}

