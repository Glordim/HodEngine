#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Color.h"
#include "Actor.h"

#include <PxPhysicsAPI.h>

class SceneComponent;
class CameraComponent;

#include "DebugLine.h"

class Scene
{
public:
    Scene();
    virtual ~Scene();

    template<typename T>
    T* spawnActor(const std::string& name);

    void drawDebugPhysics(CameraComponent* camera);

    void drawLine(glm::vec3 start, glm::vec3 end, Color color, float duration);
    void drawLine(const std::vector<Line_3P_3C>& lineVector, float duration);
    void drawTri(const std::vector<Tri_3P_3C>& triVector, float duration);

    void simulatePhysic(float dt);
    void update(float dt);

    bool raycast(glm::vec3 origin, glm::vec3 dir, float distance, physx::PxRaycastBuffer& result, bool drawDebug, Color debugColor, float debugDuration);

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
