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

void RenderQueue::AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->meshList.push_back(MeshData(mesh, materialInstance, matrix));
}

void RenderQueue::AddLines(std::vector<Line_3P_3C> lines, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->lineList.push_back(LineData(lines, materialInstance, matrix));
}

void RenderQueue::AddTriangles(std::vector<Tri_3P_3C> tris, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->triangleList.push_back(TriangleData(tris, materialInstance, matrix));
}

void RenderQueue::AddPointLight(PointLight* pointLight, glm::mat4x4 matrix)
{
    this->pointLightList.push_back(PointLightData(pointLight, matrix));
}

