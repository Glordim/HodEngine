#include "Scene.h"

#include "Component/SceneComponent.h"

#include <Core/Src/Color.h>

#include <Physics/src/Physics.h>
#include <Physics/src/Actor.h>
#include <Physics/src/Scene.h>

#include <Renderer/src/RHI/Shader.h>
#include <Renderer/src/RHI/Material.h>
#include <Renderer/src/MaterialManager.h>

#include <Renderer/src/Renderer.h>
#include <Renderer/src/RenderQueue.h>

#include <Renderer/src/Line_3P_3C.h>

#include "Component/CameraComponent.h"

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene::Scene()
			: _ambiantColor(0.25f, 0.25f, 0.25f, 1.0f)
		{
			_root = SpawnActor<Actor>("Root");
			_root->AddComponent<SceneComponent>();

			_physicScene = PHYSICS::Physics::GetInstance()->CreateScene();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene::~Scene()
		{
			PHYSICS::Physics::GetInstance()->DestroyScene(_physicScene);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PHYSICS::Scene* Scene::GetPhysicScene() const
		{
			return _physicScene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::SimulatePhysic(float dt)
		{
			if (dt <= 0.0f)
				return;

			_physicScene->Update(dt);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::Update(float dt)
		{
			auto it = _debugLines.begin();
			auto itEnd = _debugLines.end();
			while (it != itEnd)
			{
				it->second -= dt;

				if (it->second <= 0.0f)
				{
					it = _debugLines.erase(it);
					itEnd = _debugLines.end();
				}
				else
				{
					++it;
				}
			}

			size_t actorCount = _actorList.size();
			for (size_t i = 0; i < actorCount; ++i)
			{
				_actorList[i]->Update(dt);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Scene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result, bool drawDebug, const CORE::Color& debugColor, float debugDuration)
		{
			if (drawDebug == true)
			{
				AddDebugLine(origin, origin + (dir * distance), debugColor, debugDuration);
			}

			return _physicScene->Raycast(origin, dir, distance, result);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PHYSICS::Actor* Scene::CreatePhysicActor(Actor* actor)
		{
			SceneComponent* actorSceneComponent = actor->GetComponent<SceneComponent>();

			PHYSICS::Actor* physicActor = _physicScene->CreateActor();

			physicActor->SetTransform(actorSceneComponent->GetPosition(), actorSceneComponent->GetRotation(), glm::vec3(1.0f, 1.0f, 1.0f));

			_physicActorToActorMap[physicActor] = actor;

			actor->SetPhysicActor(physicActor);

			return physicActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor* Scene::ConvertPxActor(PHYSICS::Actor* physicActor)
		{
			return _physicActorToActorMap[physicActor];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SceneComponent* Scene::GetRoot() const
		{
			return _root->GetComponent<SceneComponent>();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::SetAmbiantColor(CORE::Color& color)
		{
			_ambiantColor = color;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::AddDebugLine(const glm::vec3& start, const glm::vec3& end, const CORE::Color& color, float duration)
		{
			RENDERER::Line_3P_3C line;
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

			_debugLines.push_back(std::make_pair(line, duration));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::GetDebugLines(std::vector<RENDERER::Line_3P_3C>& lines)
		{
			for (std::pair<RENDERER::Line_3P_3C, float> line : _debugLines)
			{
				lines.push_back(line.first);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::PushToRenderQueue(RENDERER::RenderQueue* renderQueue)
		{
			if (renderQueue == nullptr)
			{
				renderQueue = RENDERER::Renderer::GetInstance()->GetRenderQueue();
			}

			_root->PushToRenderQueue(*renderQueue, true);
		}
	}
}
