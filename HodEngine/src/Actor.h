#ifndef __ACTOR_HPP__
#define __ACTOR_HPP__

#include <map>
#include <typeinfo>
#include <string>

#include "Component/Component.h"
#include "Component/SceneComponent.h"

#include "PxPhysicsAPI.h"

class InputListener;
class Scene;

typedef struct CTwBar TwBar;

class Actor
{
public:
    Actor() = delete;
    Actor(const std::string& name, Scene* scene);
    Actor(const Actor& copy) = delete;
    virtual ~Actor();

    virtual void update(float dt);

    template<typename T>
    T* addComponent();

    template<typename T>
    T* getComponent() const;

    template<typename T>
    std::vector<T*> getAllComponent() const;

    virtual void setupInputListener(InputListener* inputListener);

    void setupTweakBarForAllComponent(TwBar* twBar);

    Scene* getScene() const;
    physx::PxActor* getPxActor() const;

protected:
    Scene* scene;
    physx::PxActor* pxActor;

    std::string name;

private:

    void setPxActor(physx::PxActor* pxActor);

    std::map<size_t, Component*> componentMapping;

    friend class Scene;
};

template<typename T>
T* Actor::addComponent()
{
    T* component = this->getComponent<T>();

    if (component == nullptr)
    {
        component = new T(this);
        this->componentMapping.emplace(typeid(T).hash_code(), component);
    }
    
    return component;
}

template<typename T>
T* Actor::getComponent() const
{
    auto it = this->componentMapping.find(typeid(T).hash_code());

    if (it == this->componentMapping.end())
        return nullptr;
    else
        return (T*)it->second;
}

template<typename T>
std::vector<T*> Actor::getAllComponent() const
{
    std::vector<T*> ret;

    auto it = this->componentMapping.find(typeid(T).hash_code());

    if (it != this->componentMapping.end())
        ret.push_back((T*)it->second);

    SceneComponent* sceneComponent = this->getComponent<SceneComponent>();
    if (sceneComponent != nullptr)
    {
        size_t childCount = sceneComponent->getChildCount();
        for (int i = 0; i < childCount; ++i)
        {
            std::vector<T*> componentInChild = sceneComponent->getChild(i)->getActor()->getAllComponent<T>();

            ret.insert(ret.end(), componentInChild.begin(), componentInChild.end());
        }
    }

    return ret;
}

#endif