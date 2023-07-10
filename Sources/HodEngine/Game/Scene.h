#pragma once

#include "HodEngine/Game/Actor.h"

#include <HodEngine/Core/Color.h>
#include <HodEngine/Core/Object.h>
#include <vector>
#include <utility>

namespace hod
{
	namespace CORE
	{
		struct Color;
	}

	namespace renderer
	{
		struct Line_3P_3C;
	}

	namespace physics
	{
		struct RaycastResult;
		class Scene;
		class Actor;
	}

	namespace game
	{
		class SceneComponent;
		class CameraComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Scene : public Object
		{
			REFLECTED_CLASS(Scene, Object)

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

			bool												Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, physics::RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

			physics::Actor*										CreatePhysicActor(Actor* actor);
			Actor*												ConvertPxActor(physics::Actor* physicActor);

			void												SetAmbiantColor(Color& color);
			SceneComponent*										GetRoot() const;

			physics::Scene*										GetPhysicScene() const;

			void												AddDebugLine(const glm::vec3& start, const glm::vec3& end, const Color& color, float duration);
			void												GetDebugLines(std::vector<renderer::Line_3P_3C>& lines);

			void												PushToRenderQueue(renderer::RenderQueue* renderQueue = nullptr);

			void												SerializeInDocument(Document::Node& documentNode) const;
			void												DeserializeFromDocument(const Document::Node& documentNode);

			const std::string&									GetName() const;

			const std::vector<Actor*>&							GetActors() const;

		private:

			Color												_ambiantColor;
			Actor*												_root;

			physics::Scene*										_physicScene = nullptr;

			std::vector<Actor*>									_actorList;
			std::map<physics::Actor*, Actor*>					_physicActorToActorMap;

			std::string											_name;

			//std::vector<std::pair<renderer::Line_3P_3C, float>>	_debugLines;
		};
	}
}

#include "Scene.inl"
