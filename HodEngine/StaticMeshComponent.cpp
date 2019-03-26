#include "StaticMeshComponent.h"

#include "Actor.h"
#include "SceneComponent.h"

#include "CameraComponent.h"
#include "LightComponent.h"

#include <GLFW/glfw3.h>

#include <PxPhysicsAPI.h>

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

    glBindBuffer(GL_ARRAY_BUFFER, this->mesh->getVbo(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->mesh->getVbo(1));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->mesh->getVbo(2));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->material->use();
    this->material->setFloat("time", glfwGetTime());
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
    this->material->setFloat("specularStrength", 0.0009f);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mesh->getVbo(3));
    glDrawElements(GL_TRIANGLES, this->mesh->getIndicesCount(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
