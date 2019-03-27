#include "glad\glad.h"
#include "Scene.hpp"

#include "SceneComponent.h"

#include <PxPhysicsAPI.h>

#include "VertexShader.h"
#include "FragmentShader.h"
#include "Material.h"

#include "CameraComponent.h"

Scene::Scene()
: ambiantColor(0.25f, 0.25f, 0.25f, 1.0f)
{
    this->root = this->spawnActor<Actor>("Root");
    this->root->addComponent<SceneComponent>();

    physx::PxPhysics& pxPhysx = PxGetPhysics();

    physx::PxTolerancesScale tolerancesScale = pxPhysx.getTolerancesScale();

    physx::PxSceneDesc pxSceneDesc(tolerancesScale);

    physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
    pxSceneDesc.filterShader = gDefaultFilterShader;

    physx::PxCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
    pxSceneDesc.cpuDispatcher = mCpuDispatcher;

    this->pxScene = pxPhysx.createScene(pxSceneDesc);
    this->pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
    this->pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
}

Scene::~Scene()
{
    this->pxScene->release();
}

void Scene::drawDebug(CameraComponent* camera)
{
    VertexShader unlitVertexShader;
    if (unlitVertexShader.load("Shader/UnlitColor.vert") == false)
        return;

    FragmentShader unlitFragmentShader;
    if (unlitFragmentShader.load("Shader/UnlitColor.frag") == false)
        return;

    Material* unlitMaterial = new Material(unlitVertexShader, unlitFragmentShader);
    unlitMaterial->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    unlitMaterial->setMat4("mvp", camera->getProjectionMatrix() * glm::inverse(camera->getActor()->getComponent<SceneComponent>()->getModelMatrix()));

    struct Vertext_3V_3C
    {
        float vertex[3];
        float color[3];
    };

    const physx::PxRenderBuffer& rb = this->pxScene->getRenderBuffer();
    physx::PxU32 lineCount = rb.getNbLines();

    if (lineCount != 0)
    {
        std::vector<Vertext_3V_3C> vertices;
        vertices.reserve(2 * lineCount);

        for (physx::PxU32 i = 0; i < lineCount; i++)
        {
            const physx::PxDebugLine& line = rb.getLines()[i];
            // render the line

            Vertext_3V_3C vertex;
            vertex.vertex[0] = line.pos0.x;
            vertex.vertex[1] = line.pos0.y;
            vertex.vertex[2] = line.pos0.z;
            vertex.color[0] = (line.color0 & 0xFF000000) >> 24;
            vertex.color[1] = (line.color0 & 0x00FF0000) >> 16;
            vertex.color[2] = (line.color0 & 0x0000FF00) >> 8;

            vertices.push_back(vertex);

            vertex.vertex[0] = line.pos1.x;
            vertex.vertex[1] = line.pos1.y;
            vertex.vertex[2] = line.pos1.z;
            vertex.color[0] = (line.color1 & 0xFF000000) >> 24;
            vertex.color[1] = (line.color1 & 0x00FF0000) >> 16;
            vertex.color[2] = (line.color1 & 0x0000FF00) >> 8;

            vertices.push_back(vertex);
        }

        unsigned int vao;
        unsigned int vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertext_3V_3C) * lineCount * 2, &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);

        unlitMaterial->use();
        glDrawArrays(GL_LINES, 0, lineCount * 2);
    }

    physx::PxU32 triCount = rb.getNbTriangles();

    if (triCount != 0)
    {
        std::vector<Vertext_3V_3C> vertices;
        vertices.reserve(3 * triCount);

        for (physx::PxU32 i = 0; i < triCount; i++)
        {
            const physx::PxDebugTriangle& tri = rb.getTriangles()[i];
            // render the line

            Vertext_3V_3C vertex;
            vertex.vertex[0] = tri.pos0.x;
            vertex.vertex[1] = tri.pos0.y;
            vertex.vertex[2] = tri.pos0.z;
            vertex.color[0] = (tri.color0 & 0xFF000000) >> 24;
            vertex.color[1] = (tri.color0 & 0x00FF0000) >> 16;
            vertex.color[2] = (tri.color0 & 0x0000FF00) >> 8;

            vertices.push_back(vertex);

            vertex.vertex[0] = tri.pos1.x;
            vertex.vertex[1] = tri.pos1.y;
            vertex.vertex[2] = tri.pos1.z;
            vertex.color[0] = (tri.color1 & 0xFF000000) >> 24;
            vertex.color[1] = (tri.color1 & 0x00FF0000) >> 16;
            vertex.color[2] = (tri.color1 & 0x0000FF00) >> 8;

            vertices.push_back(vertex);

            vertex.vertex[0] = tri.pos2.x;
            vertex.vertex[1] = tri.pos2.y;
            vertex.vertex[2] = tri.pos2.z;
            vertex.color[0] = (tri.color2 & 0xFF000000) >> 24;
            vertex.color[1] = (tri.color2 & 0x00FF0000) >> 16;
            vertex.color[2] = (tri.color2 & 0x0000FF00) >> 8;

            vertices.push_back(vertex);
        }

        unsigned int vao;
        unsigned int vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertext_3V_3C) * triCount * 3, &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);

        unlitMaterial->use();
        glDrawArrays(GL_TRIANGLES, 0, triCount * 3);
    }
}

void Scene::simulatePhysic(float dt)
{
    //this->pxScene->fetchResults();
    this->pxScene->simulate(dt);
}

void Scene::update(float dt)
{
    size_t actorCount = this->actorList.size();
    for (size_t i = 0; i < actorCount; ++i)
    {
        this->actorList[i]->update(dt);
    }
}

bool Scene::raycast(glm::vec3 origin, glm::vec3 dir, float distance, physx::PxRaycastBuffer& result)
{
    physx::PxVec3 pxOrigin(origin.x, origin.y, origin.z);
    physx::PxVec3 pxDir(dir.x, dir.y, dir.z);

    return this->pxScene->raycast(pxOrigin, pxDir, distance, result);
}

void Scene::addPxActor(Actor* actor, physx::PxActor* pxActor)
{
    this->pxActorToActorMap[pxActor] = actor;

    this->pxScene->addActor(*pxActor);
}

Actor* Scene::convertPxActor(physx::PxActor* pxActor)
{
    return this->pxActorToActorMap[pxActor];
}

SceneComponent* Scene::getRoot() const
{
    return this->root->getComponent<SceneComponent>();
}

void Scene::setAmbiantColor(Color& color)
{
    this->ambiantColor = color;
}