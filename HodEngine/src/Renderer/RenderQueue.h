#ifndef __RENDER_QUEUE_HPP__
#define __RENDER_QUEUE_HPP__

#include <vector>

#include "Mesh.h"
#include "Line_3P_3C.h"
#include "Tri_3P_3C.h"
#include "Light/DirLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

class MaterialInstance;
class MaterialInstance;

#include "Renderer.h"

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

    struct LineData
    {
        LineData(std::vector<Line_3P_3C>& lines, MaterialInstance* materialInstance, glm::mat4x4& matrix)
            : mesh(nullptr)
            , materialInstance(materialInstance)
            , matrix(matrix)
        {
            Renderer* renderer = Renderer::GetInstance();

            this->mesh = renderer->CreateMesh("");
            this->mesh->LoadFromLines(lines);
        }

        ~LineData()
        {
            if (this->mesh != nullptr)
                delete this->mesh;
        }

        Mesh* mesh;
        MaterialInstance* materialInstance;
        glm::mat4x4 matrix;
    };

    struct TriangleData
    {
        TriangleData(std::vector<Tri_3P_3C>& triangles, MaterialInstance* materialInstance, glm::mat4x4& matrix)
            : mesh(nullptr)
            , materialInstance(materialInstance)
            , matrix(matrix)
        {
            Renderer* renderer = Renderer::GetInstance();

            this->mesh = renderer->CreateMesh("");
            this->mesh->LoadFromTriangles(triangles);
        }

        ~TriangleData()
        {
            if (this->mesh != nullptr)
                delete this->mesh;
        }

        Mesh* mesh;
        MaterialInstance* materialInstance;
        glm::mat4x4 matrix;
    };

    struct DirLightData
    {
        DirLightData(const glm::vec3& dir, DirLight* dirLight)
            : dir(dir)
            , dirLight(dirLight)
        {

        }

        glm::vec3 dir;
        DirLight* dirLight;
    };

    struct PointLightData
    {
        PointLightData(const glm::vec3& pos, PointLight* pointLight)
            : pos(pos)
            , pointLight(pointLight)
        {

        }

        glm::vec3 pos;
        PointLight* pointLight;
    };

    struct SpotLightData
    {
        SpotLightData(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight)
            : pos(pos)
            , dir(dir)
            , spotLight(spotLight)
        {

        }

        glm::vec3 pos;
        glm::vec3 dir;
        SpotLight* spotLight;
    };

    RenderQueue();
    virtual ~RenderQueue();

    void SetViewMatrix(const glm::mat4x4& viewMatrix);
    const glm::mat4x4& GetViewMatrix() const;

    void SetProjMatrix(const glm::mat4x4& projMatrix);
    const glm::mat4x4& GetProjMatrix() const;

    void SetCameraPos(const glm::vec3& cameraPos);
    const glm::vec3 GetCameraPos() const;

    void SetClearFlag(int clearFlags);

    void SetHdriMaterial(MaterialInstance* hdriMat);
    MaterialInstance *GetHdriMaterial() const;

    void AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix);
    void AddLines(std::vector<Line_3P_3C> lines, MaterialInstance* materialInstance, glm::mat4x4 matrix);
    void AddTriangles(std::vector<Tri_3P_3C> tris, MaterialInstance* materialInstance, glm::mat4x4 matrix);

    void AddDirLight(const glm::vec3& dir, DirLight* dirLight);
    void AddPointLight(const glm::vec3& pos, PointLight* pointLight);
    void AddSpotLight(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight);

    const std::vector<MeshData*>& GetMeshDatas() const
    {
        return this->meshList;
    }

    const std::vector<LineData*>& GetLineDatas() const
    {
        return this->lineList;
    }

    const std::vector<TriangleData*>& GetTriangleDatas() const
    {
        return this->triangleList;
    }

    const std::vector<DirLightData*>& GetDirLightDatas() const
    {
        return this->dirLightList;
    }

    const std::vector<PointLightData*>& GetPointLightDatas() const
    {
        return this->pointLightList;
    }

    const std::vector<SpotLightData*>& GetSpotLightDatas() const
    {
        return this->spotLightList;
    }

private:
    glm::mat4x4 viewMatrix;
    glm::mat4x4 projMatrix;
    glm::vec3 cameraPos;

    MaterialInstance* hdriMat;

    std::vector<MeshData*> meshList;
    std::vector<LineData*> lineList;
    std::vector<TriangleData*> triangleList;

    std::vector<DirLightData*> dirLightList;
    std::vector<PointLightData*> pointLightList;
    std::vector<SpotLightData*> spotLightList;
};

#endif
