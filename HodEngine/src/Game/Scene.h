#pragma once

#include "Color.h"
#include "Actor.h"

#include "Renderer/DebugLine.h"
#include "Renderer/DebugMesh.h"

namespace HOD
{
	namespace PHYSIC
	{
		class Scene;
		class Actor;
	}

    namespace GAME
    {
        class SceneComponent;
        class CameraComponent;

        class Scene
        {
        public:
            Scene();
            virtual ~Scene();

            template<typename T>
            T* spawnActor(const std::string& name);

            void drawDebugPhysics(CameraComponent* camera, float dt);
            void drawDebugLines(CameraComponent* camera, float dt);
            void drawDebugTris(CameraComponent* camera, float dt);

            void drawLine(glm::vec3 start, glm::vec3 end, Color color, float duration);
            void drawLine(const std::vector<Line_3P_3C>& lineVector, float duration);
            void drawTri(const std::vector<Tri_3P_3C>& triVector, float duration);

            void simulatePhysic(float dt);
            void update(float dt);

            bool raycast(glm::vec3 origin, glm::vec3 dir, float distance, physx::PxRaycastBuffer& result, bool drawDebug, Color debugColor, float debugDuration);

            void CreatePhysicActor(Actor* actor);
            Actor* convertPxActor(PHYSIC::Actor* physicActor);

            void setAmbiantColor(Color& color);
            SceneComponent* getRoot() const;

			PHYSIC::Scene* GetPhysicScene() const;

        private:
            Color ambiantColor;
            Actor* root;

			PHYSIC::Scene* physicScene = nullptr;

            std::vector<Actor*> actorList;
            std::map<PHYSIC::Actor*, Actor*> physicActorToActorMap;

            std::vector<std::pair<DebugLine*, float>> debugLines;
            std::vector<DebugLine*> debugLinesImmediate;

            std::vector<std::pair<DebugMesh*, float>> debugMeshes;
            std::vector<DebugMesh*> debugMeshesImmediate;
        };

        template<typename T>
        T* Scene::spawnActor(const std::string& name)
        {
            T* actor = new T(name, this);

            this->actorList.push_back(actor);

            return actor;
        }
    }
}
