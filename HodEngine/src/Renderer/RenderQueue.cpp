#include "RenderQueue.h"

RenderQueue::RenderQueue()
{
    this->hdriMat = nullptr;
}

RenderQueue::~RenderQueue()
{
    size_t meshCount = this->meshList.size();
    for (size_t i = 0; i < meshCount; ++i)
    {
        delete this->meshList[i];
    }

    size_t lineCount = this->lineList.size();
    for (size_t i = 0; i < lineCount; ++i)
    {
        delete this->lineList[i];
    }

    size_t triCount = this->triangleList.size();
    for (size_t i = 0; i < triCount; ++i)
    {
        delete this->triangleList[i];
    }

    size_t lightCount = this->pointLightList.size();
    for (size_t i = 0; i < lightCount; ++i)
    {
        delete this->pointLightList[i];
    }
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

void RenderQueue::SetCameraPos(const glm::vec3& cameraPos)
{
    this->cameraPos = cameraPos;
}

const glm::vec3 RenderQueue::GetCameraPos() const
{
    return this->cameraPos;
}

void RenderQueue::SetClearFlag(int clearFlags)
{

}

void RenderQueue::SetHdriMaterial(MaterialInstance* hdriMat)
{
    this->hdriMat = hdriMat;
}

MaterialInstance* RenderQueue::GetHdriMaterial() const
{
    return this->hdriMat;
}

void RenderQueue::SetHdriTexture(Texture* hdriTexture)
{
    this->hdriTexture = hdriTexture;
}

Texture* RenderQueue::GetHdriTexture() const
{
    return this->hdriTexture;
}

void RenderQueue::AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->meshList.push_back(new MeshData(mesh, materialInstance, matrix));
}

void RenderQueue::AddLines(std::vector<Line_3P_3C> lines, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->lineList.push_back(new LineData(lines, materialInstance, matrix));
}

void RenderQueue::AddTriangles(std::vector<Tri_3P_3C> tris, MaterialInstance* materialInstance, glm::mat4x4 matrix)
{
    this->triangleList.push_back(new TriangleData(tris, materialInstance, matrix));
}

void RenderQueue::AddDirLight(const glm::vec3& dir, DirLight* dirLight)
{
    this->dirLightList.push_back(new DirLightData(dir, dirLight));
}

void RenderQueue::AddPointLight(const glm::vec3& pos, PointLight* pointLight)
{
    this->pointLightList.push_back(new PointLightData(pos, pointLight));
}

void RenderQueue::AddSpotLight(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight)
{
    this->spotLightList.push_back(new SpotLightData(pos, dir, spotLight));
}

