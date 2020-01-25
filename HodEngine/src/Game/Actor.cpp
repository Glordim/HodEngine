#include "Actor.h"

//#include "AntTweakBar.h"

namespace HOD
{
    namespace GAME
    {
        Actor::Actor(const std::string& name, Scene* scene)
            : name(name)
            , scene(scene)
            , physicActor(nullptr)
        {

        }

        Actor::~Actor()
        {
        }

        void Actor::setupInputListener(InputListener* inputListener)
        {

        }

        void Actor::start()
        {

        }

        void Actor::update(float dt)
        {

        }

        void Actor::DrawImGui()
        {
            
        }

        Scene* Actor::getScene() const
        {
            return this->scene;
        }

        const std::string& Actor::getName() const
        {
            return this->name;
        }

        PHYSIC::Actor* Actor::GetPhysicActor() const
        {
            return this->physicActor;
        }

        void Actor::SetPhysicActor(PHYSIC::Actor* physicActor)
        {
            this->physicActor = physicActor;
        }

        std::vector<Component*> Actor::getAllComponent() const
        {
            std::vector<Component*> vComponents;

            auto it = this->componentMapping.begin();
            auto itEnd = this->componentMapping.end();

            while (it != itEnd)
            {
                vComponents.push_back(it->second);

                ++it;
            }

            return vComponents;
        }
    }
}
