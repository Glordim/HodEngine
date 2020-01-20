#include "Scene.h"

#include "Actor.h"

namespace HOD
{
    namespace PHYSIC
    {
		Actor* Scene::CreateActor()
		{
			Actor* actor = new Actor();

			pxScene->addActor(*actor->GetPxActor());

			actors.push_back(actor);
		}

		void Scene::Update(float dt)
		{
			this->pxScene->simulate(dt);
			this->pxScene->fetchResults(true);
		}
    }
}
