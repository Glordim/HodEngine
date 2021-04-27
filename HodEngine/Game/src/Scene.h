#pragma once

#include <Core/src/Color.h>
#include "Actor.h"

#include <vector>
#include <utility>

namespace HOD
{
	namespace CORE
	{
		struct Color;
	}

	namespace RENDERER
	{
		struct Line_3P_3C;
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Scene
		{
		public:
																Scene();
																Scene(const Scene&) = delete;
																Scene(Scene&&) = delete;
			virtual												~Scene();

			void												operator=(const Scene&) = delete;
			void												operator=(Scene&&) = delete;

		public:

			template<typename T>
			T*													SpawnActor(const std::string& name);

			void												SimulatePhysic(float dt);
			void												Update(float dt);

			bool												Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration);

			PHYSICS::Actor*										CreatePhysicActor(Actor* actor);
			Actor*												ConvertPxActor(PHYSICS::Actor* physicActor);

			void												SetAmbiantColor(CORE::Color& color);
			SceneComponent*										GetRoot() const;

			PHYSICS::Scene*										GetPhysicScene() const;

			void												AddDebugLine(const glm::vec3& start, const glm::vec3& end, const CORE::Color& color, float duration);
			void												GetDebugLines(std::vector<RENDERER::Line_3P_3C>& lines);

			void												PushToRenderQueue(RENDERER::RenderQueue* renderQueue = nullptr);

		private:

			CORE::Color											_ambiantColor;
			Actor*												_root;

			PHYSICS::Scene*										_physicScene = nullptr;

			std::vector<Actor*>									_actorList;
			std::map<PHYSICS::Actor*, Actor*>					_physicActorToActorMap;

			//std::vector<std::pair<RENDERER::Line_3P_3C, float>>	_debugLines;
		};
	}
}

#include "Scene.inl"
