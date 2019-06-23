#include "StaticMeshComponent.h"

#include "Material.h"
#include "Mesh.h"

#include "Actor.h"
#include "SceneComponent.h"

#include "CameraComponent.h"
#include "LightComponent.h"

#include <PxPhysicsAPI.h>

#include <SDL.h>

StaticMeshComponent::StaticMeshComponent(Actor* actor) : RendererComponent(actor)
{

}

StaticMeshComponent::~StaticMeshComponent()
{

}

void StaticMeshComponent::setMesh(Mesh* mesh)
{
    this->mesh = mesh;
}

void StaticMeshComponent::setMaterial(Material* material)
{
    this->material = material;
}

void StaticMeshComponent::draw(CameraComponent* camera, std::vector<LightComponent*>& allLight)
{
    SceneComponent* sceneComponent = this->getActor()->getComponent<SceneComponent>();

    this->material->use();
    this->material->setFloat("time", (float)SDL_GetTicks() / 1000.0f);
    this->material->setMat4("mvp", camera->getProjectionMatrix() * glm::inverse(camera->getActor()->getComponent<SceneComponent>()->getModelMatrix()) * sceneComponent->getModelMatrix());
    this->material->setMat4("model", sceneComponent->getModelMatrix());
    size_t lightCount = allLight.size();
    this->material->setInt("lightCount", lightCount);

    for (int i = 0; i < lightCount; ++i)
    {
        LightComponent* light = allLight[i];

        this->material->setFloat("pointLight[" + std::to_string(i) + "].intensity", light->intensity);
        this->material->setFloat("pointLight[" + std::to_string(i) + "].range", light->range);
        this->material->setVec4("pointLight[" + std::to_string(i) + "].color", glm::vec4(light->color.r, light->color.g, light->color.b, light->color.a));
        this->material->setVec4("pointLight[" + std::to_string(i) + "].pos", glm::vec4(light->getActor()->getComponent<SceneComponent>()->getPosition(), 1.0f));
    }

    this->material->setVec4("ambiantColor", glm::vec4(0.25f, 0.25f, 0.5f, 1.0f));
    this->material->setVec4("eyePos", glm::vec4(camera->getActor()->getComponent<SceneComponent>()->getPosition(), 1.0f));
    this->material->setFloat("shininess", 32);
    this->material->setFloat("specularStrength", 1.5f);

    glBindVertexArray(this->mesh->getVao());
    glDrawElements(GL_TRIANGLES, this->mesh->getIndicesCount(), GL_UNSIGNED_INT, 0);
}
