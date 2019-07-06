#include "glad\glad.h"
#include "Scene.hpp"

#include "SceneComponent.h"

#include <PxPhysicsAPI.h>

#include "Shader.h"
#include "Material.h"
#include "MaterialManager.h"

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
    this->pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
    this->pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

    this->pxDefaultMaterial = pxPhysx.createMaterial(0.0f, 0.0f, 0.0f);
}

Scene::~Scene()
{
    this->pxScene->release();
    this->pxDefaultMaterial->release();
}

physx::PxScene* Scene::GetPxScene() const
{
    return this->pxScene;
}

physx::PxMaterial* Scene::getDefaultMaterial() const
{
    return this->pxDefaultMaterial;
}

void Scene::drawDebugPhysics(CameraComponent* camera, float dt)
{
    MaterialManager* materialManager = MaterialManager::getInstance();
    Material* material = materialManager->getMaterial("UnlitVertexColor");
    material->use();
    material->setMat4("mvp", camera->getProjectionMatrix() * glm::inverse(camera->getActor()->getComponent<SceneComponent>()->getModelMatrix()));

    const physx::PxRenderBuffer& rb = this->pxScene->getRenderBuffer();
    physx::PxU32 lineCount = rb.getNbLines();

    if (lineCount != 0)
    {
        std::vector<Line_3P_3C> lines;
        lines.resize(lineCount);

        for (physx::PxU32 i = 0; i < lineCount; i++)
        {
            const physx::PxDebugLine& pxLine = rb.getLines()[i];

            Line_3P_3C& line = lines[i];

            line.vertices[0].pos[0] = pxLine.pos0.x;
            line.vertices[0].pos[1] = pxLine.pos0.y;
            line.vertices[0].pos[2] = pxLine.pos0.z;
            line.vertices[0].color[0] = (pxLine.color0 & 0x00FF0000) >> 16;
            line.vertices[0].color[1] = (pxLine.color0 & 0x0000FF00) >> 8;
            line.vertices[0].color[2] = (pxLine.color0 & 0x000000FF) >> 0;

            line.vertices[1].pos[0] = pxLine.pos1.x;
            line.vertices[1].pos[1] = pxLine.pos1.y;
            line.vertices[1].pos[2] = pxLine.pos1.z;
            line.vertices[1].color[0] = (pxLine.color1 & 0x00FF0000) >> 16;
            line.vertices[1].color[1] = (pxLine.color1 & 0x0000FF00) >> 8;
            line.vertices[1].color[2] = (pxLine.color1 & 0x000000FF) >> 0;
        }

        this->drawLine(lines, 0.0f);
    }

    physx::PxU32 triCount = rb.getNbTriangles();

    if (triCount != 0)
    {
        std::vector<Tri_3P_3C> tris;
        tris.resize(triCount);

        for (physx::PxU32 i = 0; i < triCount; i++)
        {
            const physx::PxDebugTriangle& pxTri = rb.getTriangles()[i];
            // render the line

            Tri_3P_3C& tri = tris[i];

            tri.vertices[0].pos[0] = pxTri.pos0.x;
            tri.vertices[0].pos[1] = pxTri.pos0.y;
            tri.vertices[0].pos[2] = pxTri.pos0.z;
            tri.vertices[0].color[0] = (pxTri.color0 & 0x00FF0000) >> 16;
            tri.vertices[0].color[1] = (pxTri.color0 & 0x0000FF00) >> 8;
            tri.vertices[0].color[2] = (pxTri.color0 & 0x000000FF) >> 0;

            tri.vertices[1].pos[0] = pxTri.pos1.x;
            tri.vertices[1].pos[1] = pxTri.pos1.y;
            tri.vertices[1].pos[2] = pxTri.pos1.z;
            tri.vertices[1].color[0] = (pxTri.color1 & 0x00FF0000) >> 16;
            tri.vertices[1].color[1] = (pxTri.color1 & 0x0000FF00) >> 8;
            tri.vertices[1].color[2] = (pxTri.color1 & 0x000000FF) >> 0;

            tri.vertices[2].pos[0] = pxTri.pos2.x;
            tri.vertices[2].pos[1] = pxTri.pos2.y;
            tri.vertices[2].pos[2] = pxTri.pos2.z;
            tri.vertices[2].color[0] = (pxTri.color2 & 0x00FF0000) >> 16;
            tri.vertices[2].color[1] = (pxTri.color2 & 0x0000FF00) >> 8;
            tri.vertices[2].color[2] = (pxTri.color2 & 0x000000FF) >> 0;
        }

        this->drawTri(tris, 0.0f);
    }

    this->drawDebugLines(camera, dt);
    this->drawDebugTris(camera, dt);
}

void Scene::drawDebugLines(CameraComponent* camera, float dt)
{
    auto it = this->debugLines.begin();
    auto itEnd = this->debugLines.end();
    while (it != itEnd)
    {
        it->first->draw(camera);
        it->second -= dt;

        if (it->second <= 0.0f)
        {
            delete it->first;
            it = this->debugLines.erase(it);
            itEnd = this->debugLines.end();
        }
        else
        {
            ++it;
        }
    }

    size_t debugLineImmediateCount = this->debugLinesImmediate.size();
    for (int i = 0; i < debugLineImmediateCount; ++i)
    {
        DebugLine* debugLine = this->debugLinesImmediate[i];
        debugLine->draw(camera);
        delete debugLine;
    }
    this->debugLinesImmediate.clear();
}

void Scene::drawDebugTris(CameraComponent* camera, float dt)
{
    auto it = this->debugMeshes.begin();
    auto itEnd = this->debugMeshes.end();
    while (it != itEnd)
    {
        it->first->draw(camera);
        it->second -= dt;

        if (it->second <= 0.0f)
        {
            delete it->first;
            it = this->debugMeshes.erase(it);
            itEnd = this->debugMeshes.end();
        }
        else
        {
            ++it;
        }
    }

    size_t debugMeshesImmediateCount = this->debugMeshesImmediate.size();
    for (int i = 0; i < debugMeshesImmediateCount; ++i)
    {
        DebugMesh* debugMesh = this->debugMeshesImmediate[i];
        debugMesh->draw(camera);
        delete debugMesh;
    }
    this->debugMeshesImmediate.clear();
}

void Scene::drawLine(glm::vec3 start, glm::vec3 end, Color color, float duration)
{
    std::vector<Line_3P_3C> lines;

    Line_3P_3C line;

    line.vertices[0].pos[0] = start.x;
    line.vertices[0].pos[1] = start.y;
    line.vertices[0].pos[2] = start.z;
    line.vertices[0].color[0] = color.r;
    line.vertices[0].color[1] = color.g;
    line.vertices[0].color[2] = color.b;

    line.vertices[1].pos[0] = end.x;
    line.vertices[1].pos[1] = end.y;
    line.vertices[1].pos[2] = end.z;
    line.vertices[1].color[0] = color.r;
    line.vertices[1].color[1] = color.g;
    line.vertices[1].color[2] = color.b;

    lines.push_back(line);

    this->drawLine(lines, duration);
}

void Scene::drawLine(const std::vector<Line_3P_3C>& lineVector, float duration)
{
    DebugLine* debugLine = new DebugLine();
    debugLine->buildVao(lineVector);

    if (duration > 0)
        this->debugLines.push_back(std::make_pair(debugLine, duration));
    else
        this->debugLinesImmediate.push_back(debugLine);
}

void Scene::drawTri(const std::vector<Tri_3P_3C>& triVector, float duration)
{
    DebugMesh* debugMesh = new DebugMesh();
    debugMesh->buildVao(triVector);

    if (duration > 0)
        this->debugMeshes.push_back(std::make_pair(debugMesh, duration));
    else
        this->debugMeshesImmediate.push_back(debugMesh);
}

void Scene::simulatePhysic(float dt)
{
    if (dt <= 0.0f)
        return;

    this->pxScene->simulate(dt);    
    this->pxScene->fetchResults(true);
}

void Scene::update(float dt)
{
    size_t actorCount = this->actorList.size();
    for (size_t i = 0; i < actorCount; ++i)
    {
        this->actorList[i]->update(dt);
    }
}

bool Scene::raycast(glm::vec3 origin, glm::vec3 dir, float distance, physx::PxRaycastBuffer& result, bool drawDebug, Color debugColor, float debugDuration)
{
    if (drawDebug == true)
        this->drawLine(origin, origin + (dir * distance), debugColor, debugDuration);

    physx::PxVec3 pxOrigin(origin.x, origin.y, origin.z);
    physx::PxVec3 pxDir(dir.x, dir.y, dir.z);

    pxDir.normalize();

    return this->pxScene->raycast(pxOrigin, pxDir, distance, result);
}

void Scene::addPxActor(Actor* actor, physx::PxActor* pxActor)
{
    actor->setPxActor(pxActor);

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