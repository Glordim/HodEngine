#include "RenderQueue.h"

RenderQueue::RenderQueue()
{
}

RenderQueue::~RenderQueue()
{
}

void RenderQueue::SetViewMatrix(const glm::mat4x4& viewMatrix)
{
    this->viewMatrix = viewMatrix;
}

const glm::mat4x4& RenderQueue::GetViewMatrix() const
{
    return this->viewMatrix;
}

void RenderQueue::SetProjMatrix(const glm::mat4x4& projMatrix)
{
    this->projMatrix = projMatrix;
}

const glm::mat4x4& RenderQueue::GetProjMatrix() const
{
    return this->projMatrix;
}

void RenderQueue::SetClearFlag(int clearFlags)
{

}

void RenderQueue::AddMesh(Mesh* mesh, Material* material, glm::mat4x4 matrix)
{
    this->meshList.push_back(MeshData(mesh, material, matrix));
}

void RenderQueue::AddPointLight(PointLight* pointLight, glm::mat4x4 matrix)
{
    this->pointLightList.push_back(PointLightData(pointLight, matrix));
}

