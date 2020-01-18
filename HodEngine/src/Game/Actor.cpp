#include "Actor.h"

//#include "AntTweakBar.h"

namespace HOD
{
    namespace GAME
    {
        Actor::Actor(const std::string& name, Scene* scene)
            : name(name)
            , scene(scene)
            , pxActor(nullptr)
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

        physx::PxActor* Actor::getPxActor() const
        {
            return this->pxActor;
        }

        void Actor::setPxActor(physx::PxActor* pxActor)
        {
            this->pxActor = pxActor;
        }
    }
}
