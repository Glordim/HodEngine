#include "Scene.hpp"

#include "SceneComponent.h"

#include <PxPhysicsAPI.h>

Scene::Scene()
: ambiantColor(0.25f, 0.25f, 0.25f, 1.0f)
{
    this->root = this->spawnActor<Actor>("Root");
    this->root->addComponent<SceneComponent>();

    physx::PxPhysics& pxPhysx = PxGetPhysics();

    physx::PxTolerancesScale tolerancesScale = pxPhysx.getTolerancesScale();

    physx::PxSceneDesc pxSceneDesc(tolerancesScale);

    this->pxScene = pxPhysx.createScene(pxSceneDesc);
}

Scene::~Scene()
{
    this->pxScene->release();
}

void Scene::simulatePhysic(float dt)
{
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