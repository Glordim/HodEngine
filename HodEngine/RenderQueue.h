#ifndef __RENDER_QUEUE_HPP__
#define __RENDER_QUEUE_HPP__

#include <vector>

#include "Mesh.h"
#include "PointLight.h"
#include "glm/mat4x4.hpp"

class MaterialInstance;

class RenderQueue
{
public:

    enum ClearFlag
    {
        NONE,
        COLOR,
        DEPTH
    };

    struct MeshData
    {
        MeshData(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix)
            : mesh(mesh)
            , materialInstance(materialInstance)
            , matrix(matrix)
        {

        }

        Mesh* mesh;
        MaterialInstance* materialInstance;
        glm::mat4x4 matrix;
    };

    struct PointLightData
    {
        PointLightData(PointLight* pointLight, glm::mat4x4 matrix)
            : pointLight(pointLight)
            , matrix(matrix)
        {

        }

        PointLight* pointLight;
        glm::mat4x4 matrix;
    };

    RenderQueue();
    virtual ~RenderQueue();

    void SetViewMatrix(const glm::mat4x4& viewMatrix);
    const glm::mat4x4& GetViewMatrix() const;

    void SetProjMatrix(const glm::mat4x4& projMatrix);
    const glm::mat4x4& GetProjMatrix() const;

    void SetClearFlag(int clearFlags);

    void AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix);

    void AddPointLight(PointLight* pointLight, glm::mat4x4 matrix);
    //void AddDirectionnalLight(const DirectionnalLight dirLight);
    //void AddSpotLight(const SpotLight& spotLight);

    std::vector<MeshData> GetMeshDatas()
    {
        return this->meshList;
    }

private:
    glm::mat4x4 viewMatrix;
    glm::mat4x4 projMatrix;

    std::vector<MeshData> meshList;
    std::vector<PointLightData> pointLightList;
};

#endif
