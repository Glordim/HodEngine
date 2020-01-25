#include "Scene.h"

#include "Component/SceneComponent.h"

#include <Physic/src/Physic.h>
#include <Physic/src/Actor.h>
#include <Physic/src/Scene.h>

#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/MaterialManager.h"

#include "Component/CameraComponent.h"

namespace HOD
{
    namespace GAME
    {
        Scene::Scene()
            : ambiantColor(0.25f, 0.25f, 0.25f, 1.0f)
        {
            this->root = this->spawnActor<Actor>("Root");
            this->root->addComponent<SceneComponent>();

			this->physicScene = PHYSIC::Physic::GetInstance()->CreateScene();
        }

        Scene::~Scene()
        {
            PHYSIC::Physic::GetInstance()->DestroyScene(this->physicScene);
        }

        PHYSIC::Scene* Scene::GetPhysicScene() const
        {
            return this->physicScene;
        }

        void Scene::simulatePhysic(float dt)
        {
            if (dt <= 0.0f)
                return;

			this->physicScene->Update(dt);
        }

        void Scene::update(float dt)
        {
            size_t actorCount = this->actorList.size();
            for (size_t i = 0; i < actorCount; ++i)
            {
                this->actorList[i]->update(dt);
            }
        }

        bool Scene::raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration)
        {
            //if (drawDebug == true)
            //    this->drawLine(origin, origin + (dir * distance), debugColor, debugDuration);

			return physicScene->Raycast(origin, dir, distance, result);
        }

        PHYSIC::Actor* Scene::CreatePhysicActor(Actor* actor)
        {
			SceneComponent* actorSceneComponent = actor->getComponent<SceneComponent>();

			PHYSIC::Actor* physicActor = physicScene->CreateActor();

			physicActor->SetTransform(actorSceneComponent->getPosition(), actorSceneComponent->getRotation(), glm::vec3(1.0f, 1.0f, 1.0f));

            this->physicActorToActorMap[physicActor] = actor;

            actor->SetPhysicActor(physicActor);

            return physicActor;
        }

        Actor* Scene::convertPxActor(PHYSIC::Actor* physicActor)
        {
            return this->physicActorToActorMap[physicActor];
        }

        SceneComponent* Scene::getRoot() const
        {
            return this->root->getComponent<SceneComponent>();
        }

        void Scene::setAmbiantColor(CORE::Color& color)
        {
            this->ambiantColor = color;
        }
    }
}
