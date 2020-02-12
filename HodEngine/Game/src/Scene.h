#pragma once

#include <Core/src/Color.h>
#include "Actor.h"

#include <Renderer/src/Line_3P_3C.h>
#include <Renderer/src/Tri_3P_3C.h>

#include <vector>
#include <utility>

namespace HOD
{
	namespace CORE
	{
		struct Color;
	}

	namespace PHYSICS
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
			
            bool raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration);

			PHYSICS::Actor* CreatePhysicActor(Actor* actor);
            Actor* convertPxActor(PHYSICS::Actor* physicActor);

            void setAmbiantColor(CORE::Color& color);
            SceneComponent* getRoot() const;

			PHYSICS::Scene* GetPhysicScene() const;

			void AddDebugLine(const glm::vec3& start, const glm::vec3& end, const CORE::Color& color, float duration);
			void GetDebugLines(std::vector<Line_3P_3C>& lines);

        private:
            CORE::Color ambiantColor;
            Actor* root;

			PHYSICS::Scene* physicScene = nullptr;

            std::vector<Actor*> actorList;
            std::map<PHYSICS::Actor*, Actor*> physicActorToActorMap;

			std::vector<std::pair<Line_3P_3C, float>> vDebugLines;
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
