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

        void Actor::setupTweakBarForAllComponent(TwBar* twBar)
        {
            //TwRemoveAllVars(twBar);

            auto it = this->componentMapping.begin();
            auto itEnd = this->componentMapping.end();

            while (it != itEnd)
            {
                it->second->setupTweakBar(twBar);

                ++it;
            }
        }

        Scene* Actor::getScene() const
        {
            return this->scene;
        }

        PHYSIC::Actor* Actor::GetPhysicActor() const
        {
            return this->physicActor;
        }

        void Actor::SetPhysicActor(PHYSIC::Actor* physicActor)
        {
            this->physicActor = physicActor;
        }
    }
}
