#include "Scene.h"

#include "Component/SceneComponent.h"

#include "Physic/Physic.h"
#include "Physic/Scene.h"
#include "Physic/Actor.h"

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

        void Scene::drawDebugPhysics(CameraComponent* camera, float dt)
        {
			std::vector<Line_3P_3C> lines;
			std::vector<Tri_3P_3C> tris;
			physicScene->GetDebugGeometry(lines, tris);

			this->drawLine(lines, 0.0f);
			this->drawTri(tris, 0.0f);

            this->drawDebugLines(camera, dt);
            this->drawDebugTris(camera, dt);
        }

        void Scene::drawDebugLines(CameraComponent* camera, float dt)
        {
            auto it = this->debugLines.begin();
            auto itEnd = this->debugLines.end();
            while (it != itEnd)
            {
                it->first->draw(camera);
                it->second -= dt;

                if (it->second <= 0.0f)
                {
                    delete it->first;
                    it = this->debugLines.erase(it);
                    itEnd = this->debugLines.end();
                }
                else
                {
                    ++it;
                }
            }

            size_t debugLineImmediateCount = this->debugLinesImmediate.size();
            for (int i = 0; i < debugLineImmediateCount; ++i)
            {
                DebugLine* debugLine = this->debugLinesImmediate[i];
                debugLine->draw(camera);
                delete debugLine;
            }
            this->debugLinesImmediate.clear();
        }

        void Scene::drawDebugTris(CameraComponent* camera, float dt)
        {
            auto it = this->debugMeshes.begin();
            auto itEnd = this->debugMeshes.end();
            while (it != itEnd)
            {
                it->first->draw(camera);
                it->second -= dt;

                if (it->second <= 0.0f)
                {
                    delete it->first;
                    it = this->debugMeshes.erase(it);
                    itEnd = this->debugMeshes.end();
                }
                else
                {
                    ++it;
                }
            }

            size_t debugMeshesImmediateCount = this->debugMeshesImmediate.size();
            for (int i = 0; i < debugMeshesImmediateCount; ++i)
            {
                DebugMesh* debugMesh = this->debugMeshesImmediate[i];
                debugMesh->draw(camera);
                delete debugMesh;
            }
            this->debugMeshesImmediate.clear();
        }

        void Scene::drawLine(glm::vec3 start, glm::vec3 end, Color color, float duration)
        {
            std::vector<Line_3P_3C> lines;

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

            lines.push_back(line);

            this->drawLine(lines, duration);
        }

        void Scene::drawLine(const std::vector<Line_3P_3C>& lineVector, float duration)
        {
            DebugLine* debugLine = new DebugLine();
            debugLine->buildVao(lineVector);

            if (duration > 0)
                this->debugLines.push_back(std::make_pair(debugLine, duration));
            else
                this->debugLinesImmediate.push_back(debugLine);
        }

        void Scene::drawTri(const std::vector<Tri_3P_3C>& triVector, float duration)
        {
            DebugMesh* debugMesh = new DebugMesh();
            debugMesh->buildVao(triVector);

            if (duration > 0)
                this->debugMeshes.push_back(std::make_pair(debugMesh, duration));
            else
                this->debugMeshesImmediate.push_back(debugMesh);
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

        bool Scene::raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration)
        {
            if (drawDebug == true)
                this->drawLine(origin, origin + (dir * distance), debugColor, debugDuration);

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

        void Scene::setAmbiantColor(Color& color)
        {
            this->ambiantColor = color;
        }
    }
}
