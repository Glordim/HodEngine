#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Color.h"
#include "Actor.h"

#include <PxPhysicsAPI.h>

class SceneComponent;
class CameraComponent;

class Scene
{
public:
    Scene();
    virtual ~Scene();

    template<typename T>
    T* spawnActor(const std::string& name);

    void drawDebug(CameraComponent* camera);

    void simulatePhysic(float dt);
    void update(float dt);

    bool raycast(glm::vec3 origin, glm::vec3 dir, float distance, physx::PxRaycastBuffer& result);

    void addPxActor(Actor* actor, physx::PxActor* pxActor);
    Actor* convertPxActor(physx::PxActor* pxActor);

    void setAmbiantColor(Color& color);
    SceneComponent* getRoot() const;

private:
    Color ambiantColor;
    Actor* root;

    physx::PxScene* pxScene;

    std::vector<Actor*> actorList;
    std::map<physx::PxActor*, Actor*> pxActorToActorMap;
};

template<typename T>
T* Scene::spawnActor(const std::string& name)
{
    T* actor = new T(name, this);

    this->actorList.push_back(actor);

    return actor;
}

#endif
