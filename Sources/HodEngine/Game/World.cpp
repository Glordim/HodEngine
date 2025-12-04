#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/Scene.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/World.hpp>

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/Context.hpp>
#include <HodEngine/Window/DisplayManager.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>
#include <HodEngine/Window/Window.hpp>

#undef min

namespace hod
{
	namespace game
	{
		// TODO
		/// @brief
		void World::DisableDrawJob()
		{
			FrameSequencer::GetInstance()->RemoveJob(&_drawJob, FrameSequencer::Step::Render);
			_drawJobEnabled = false;
		}

		void World::SetEditorPlaying(bool editorPlaying)
		{
			_editorPlaying = editorPlaying;
		}

		/// @brief
		/// @return
		bool World::GetEditorPlaying() const
		{
			return _editorPlaying;
		}

		/// @brief
		/// @param editorPaused
		void World::SetEditorPaused(bool editorPaused)
		{
			_editorPaused = editorPaused;
		}

		/// @brief
		/// @return
		bool World::GetEditorPaused() const
		{
			return _editorPaused;
		}

		/// @brief
		void World::EditorNextFrame()
		{
			_editorNextFrame = true;
		}

		//

		/// @brief
		/// @param
		World::World()
		: _updateJob(this, &World::Update, JobQueue::Queue::Framed)
		, _drawJob(this, &World::Draw, JobQueue::Queue::Framed)
		, _persistanteScene(DefaultAllocator::GetInstance().New<Scene>())
		{
		}

		/// @brief
		World::~World()
		{
			if (_jobInserted)
			{
				FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::Logic);
				if (_drawJobEnabled == true)
				{
					FrameSequencer::GetInstance()->RemoveJob(&_drawJob, FrameSequencer::Step::Render);
				}
			}

			Clear();

			DefaultAllocator::GetInstance().Delete(_persistanteScene);
			DefaultAllocator::GetInstance().Delete(_physicsWorld);
		}

		/// @brief
		/// @return
		bool World::Init()
		{
			_physicsWorld = physics::Physics::GetInstance()->CreateWorld();

			_lastUpdateTimestamp = SystemTime::Now();

			FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::Logic);
			FrameSequencer::GetInstance()->InsertJob(&_drawJob, FrameSequencer::Step::Render);
			_jobInserted = true;

			return true;
		}

		/// @brief
		void World::Clear()
		{
			for (Scene* scene : _scenes)
			{
				DefaultAllocator::GetInstance().Delete(scene);
			}
			_scenes.Clear();

			_persistanteScene->Clear();
		}

		/// @brief
		/// @return
		World* World::Clone()
		{
			World* clone = DefaultAllocator::GetInstance().New<World>();
			clone->Init();

			DefaultAllocator::GetInstance().Delete(clone->_persistanteScene);
			clone->_persistanteScene = _persistanteScene->Clone(clone);

			for (Scene* scene : _scenes)
			{
				Scene* sceneClone = scene->Clone(clone);

				clone->AddScene(sceneClone);
			}

			return clone;
		}

		/// @brief
		void World::ProcessActication()
		{
			_lastUpdateTimestamp = SystemTime::Now();

			_persistanteScene->ProcessActivation();
			for (Scene* scene : _scenes)
			{
				scene->ProcessActivation();
			}
		}

		/// @brief
		/// @return
		Scene* World::CreateScene()
		{
			Scene* pScene = DefaultAllocator::GetInstance().New<Scene>();

			_scenes.push_back(pScene);

			return pScene;
		}

		/// @brief
		/// @param pScene
		void World::DestroyScene(Scene* pScene)
		{
			auto it = _scenes.Begin();
			auto itEnd = _scenes.End();
			while (it != itEnd)
			{
				if (*it == pScene)
				{
					_scenes.Erase(it);
					DefaultAllocator::GetInstance().Delete(pScene);
					return;
				}
			}

			// Todo message not found
		}

		/// @brief
		/// @param scene
		/// @return
		bool World::AddScene(Scene* scene)
		{
			auto it = _scenes.Begin();
			auto itEnd = _scenes.End();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					// Todo message already
					return false;
				}
				++it;
			}

			scene->SetWorld(this);

			_scenes.push_back(scene);
			return true;
		}

		/// @brief
		/// @param scene
		/// @return
		bool World::RemoveScene(Scene* scene)
		{
			auto it = _scenes.Begin();
			auto itEnd = _scenes.End();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					_scenes.Erase(it);
					return true;
				}
				++it;
			}

			scene->SetWorld(nullptr);

			// Todo message not found
			return false;
		}

		/// @brief
		/// @param start
		/// @param end
		/// @param color
		/// @param duration
		void World::DrawDebugLine(const Vector2& start, const Vector2& end, const Color& color, float duration)
		{
			_debugDrawer.AddLine(start, end, color, duration);
		}

		/// @brief
		void World::Update()
		{
			// todo
			if (_editorPlaying == false || (_editorPaused == true && _editorNextFrame == false))
			{
				return;
			}

			SystemTime::TimeStamp now = SystemTime::Now();
			float                 deltaTime = (float)SystemTime::ElapsedTimeInSeconds(_lastUpdateTimestamp, now);
			_lastUpdateTimestamp = now;

			if (_editorNextFrame == true)
			{
				deltaTime = std::min(deltaTime, 0.016f);
				_editorNextFrame = false;
			}
			//

			_accumulatedPhysicsTime += deltaTime;
			while (_accumulatedPhysicsTime >= _physicsUpdateTimestep)
			{
				_accumulatedPhysicsTime -= _physicsUpdateTimestep;
				for (Scene* scene : _scenes)
				{
					scene->FixedUpdate();
				}
				_physicsWorld->Update(_physicsUpdateTimestep);
			}

			for (Scene* scene : _scenes)
			{
				scene->Update(deltaTime);
			}
		}

		/// @brief
		void World::Draw()
		{
			renderer::RenderView* renderView = renderer::Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
			renderView->Init();
			renderView->Prepare(window::DisplayManager::GetInstance()->GetMainWindow());
			Draw(*renderView);
		}

		/// @brief
		/// @param renderQueue
		void World::Draw(renderer::RenderView& renderView)
		{
			if (_editorPlaying == true && _editorPaused == false)
			{
				CameraComponent* camera = CameraComponent::_main;
				if (camera == nullptr)
				{
					static CameraComponent builtinCamera;
					camera = &builtinCamera;
				}
				camera->SetupRenderView(renderView);
			}

			for (Scene* scene : _scenes)
			{
				scene->Draw(renderView);
			}
			_persistanteScene->Draw(renderView);

			_debugDrawer.Draw(renderView);
		}

		/// @brief
		/// @return
		const Vector<Scene*>& World::GetScenes() const
		{
			return _scenes;
		}

		/// @brief
		/// @param name
		/// @return
		Entity* World::CreateEntity(const std::string_view& name)
		{
			return _persistanteScene->CreateEntity(name);
		}

		/// @brief
		/// @param entity
		void World::DestroyEntity(Entity* entity)
		{
			for (Scene* scene : _scenes)
			{
				scene->DestroyEntity(entity);
			}
			_persistanteScene->DestroyEntity(entity);

			DefaultAllocator::GetInstance().Delete(entity);
		}

		/// @brief
		/// @param entityId
		/// @return
		Entity* World::FindEntity(uint64_t entityId)
		{
			Entity* entity;

			for (Scene* scene : _scenes)
			{
				entity = scene->FindEntity(entityId);
				if (entity != nullptr)
				{
					return entity;
				}
			}
			return _persistanteScene->FindEntity(entityId);
		}

		/// @brief
		/// @return
		physics::World* World::GetPhysicsWorld() const
		{
			return _physicsWorld;
		}
	}
}
