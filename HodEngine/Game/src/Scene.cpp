#include "Scene.h"

#include "Component/SceneComponent.h"

#include <Core/Src/Color.h>

#include <Physics/src/Physics.h>
#include <Physics/src/Actor.h>
#include <Physics/src/Scene.h>

#include <Renderer/src/Shader.h>
#include <Renderer/src/Material.h>
#include <Renderer/src/MaterialManager.h>

#include <Renderer/src/Line_3P_3C.h>

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

			this->physicScene = PHYSICS::Physics::GetInstance()->CreateScene();
        }

        Scene::~Scene()
        {
			PHYSICS::Physics::GetInstance()->DestroyScene(this->physicScene);
        }

		PHYSICS::Scene* Scene::GetPhysicScene() const
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
			auto it = vDebugLines.begin();
			auto itEnd = vDebugLines.end();
			while (it != itEnd)
			{
				it->second -= dt;

				if (it->second <= 0.0f)
				{
					it = vDebugLines.erase(it);
					itEnd = vDebugLines.end();
				}
				else
				{
					++it;
				}
			}

            size_t actorCount = this->actorList.size();
            for (size_t i = 0; i < actorCount; ++i)
            {
                this->actorList[i]->update(dt);
            }
        }

        bool Scene::raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration)
        {
            if (drawDebug == true)
                this->AddDebugLine(origin, origin + (dir * distance), debugColor, debugDuration);

			return physicScene->Raycast(origin, dir, distance, result);
        }

		PHYSICS::Actor* Scene::CreatePhysicActor(Actor* actor)
        {
			SceneComponent* actorSceneComponent = actor->getComponent<SceneComponent>();

			PHYSICS::Actor* physicActor = physicScene->CreateActor();

			physicActor->SetTransform(actorSceneComponent->getPosition(), actorSceneComponent->getRotation(), glm::vec3(1.0f, 1.0f, 1.0f));

            this->physicActorToActorMap[physicActor] = actor;

            actor->SetPhysicActor(physicActor);

            return physicActor;
        }

        Actor* Scene::convertPxActor(PHYSICS::Actor* physicActor)
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

		void Scene::AddDebugLine(const glm::vec3& start, const glm::vec3& end, const CORE::Color& color, float duration)
		{
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

			vDebugLines.push_back(std::make_pair(line, duration));
		}

		void Scene::GetDebugLines(std::vector<Line_3P_3C>& lines)
		{
			for (std::pair<Line_3P_3C, float> line : vDebugLines)
			{
				lines.push_back(line.first);
			}
		}
    }
}
