#pragma once

#include "Color.h"
#include "Actor.h"

#include "PxPhysicsAPI.h"

#include "Renderer/DebugLine.h"
#include "Renderer/DebugMesh.h"

namespace HOD
{
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

            void addPxActor(Actor* actor, physx::PxActor* pxActor);
            Actor* convertPxActor(physx::PxActor* pxActor);

            void setAmbiantColor(Color& color);
            SceneComponent* getRoot() const;

            physx::PxScene* GetPxScene() const;

            physx::PxMaterial* getDefaultMaterial() const;

        private:
            Color ambiantColor;
            Actor* root;

            physx::PxScene* pxScene;
            physx::PxMaterial* pxDefaultMaterial;

            std::vector<Actor*> actorList;
            std::map<physx::PxActor*, Actor*> pxActorToActorMap;

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
