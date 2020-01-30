#pragma once

#include <Core/src/Color.h>
#include "Actor.h"

#include <Renderer/src/Line_3P_3C.h>
#include <Renderer/src/Tri_3P_3C.h>

namespace HOD
{
	namespace PHYSIC
	{
		struct RaycastResult;
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

            void simulatePhysic(float dt);
            void update(float dt);
			
            bool raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration);

            PHYSIC::Actor* CreatePhysicActor(Actor* actor);
            Actor* convertPxActor(PHYSIC::Actor* physicActor);

            void setAmbiantColor(CORE::Color& color);
            SceneComponent* getRoot() const;

			PHYSIC::Scene* GetPhysicScene() const;

        private:
            CORE::Color ambiantColor;
            Actor* root;

			PHYSIC::Scene* physicScene = nullptr;

            std::vector<Actor*> actorList;
            std::map<PHYSIC::Actor*, Actor*> physicActorToActorMap;
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
