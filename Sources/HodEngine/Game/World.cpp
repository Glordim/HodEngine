#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Builtin.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Physics/Physics.hpp>

#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>
#include <HodEngine/Window/Window.hpp>

namespace hod
{
	namespace game
	{
	// TODO
		/// @brief
		void World::DisableDrawJob()
		{
			_drawJobEnabled = false;
		}

		void World::SetEditorPlaying(bool editorPlaying)
		{
			if (_editorPlaying != editorPlaying)
			{
				_editorPlaying = editorPlaying;
				if (editorPlaying == true)
				{
					_persistanteScene->Clear();
					std::vector<Scene*> clonedScene;
					for (Scene* scene : _scenes)
					{
						Scene* clone = scene->Clone();
						clonedScene.push_back(clone);
					}
					_scenes = clonedScene;					

					for (Scene* scene : _scenes)
					{
						scene->Awake();
					}

					for (Scene* scene : _scenes)
					{
						scene->Start();
					}
				}
			}
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
		_SingletonConstructor(World)
		: _updateJob(this, &World::Update, JobQueue::Queue::FramedNormalPriority)
		, _drawJob(this, &World::Draw, JobQueue::Queue::FramedNormalPriority)
		, _persistanteScene(new Scene())
		{
		}

		/// @brief 
		World::~World()
		{
			FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::Logic);
			FrameSequencer::GetInstance()->RemoveJob(&_drawJob, FrameSequencer::Step::Render);

			Clear();

			delete _persistanteScene;
		}

		/// @brief 
		/// @return 
		bool World::Init()
		{
			FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::Logic);
			FrameSequencer::GetInstance()->InsertJob(&_drawJob, FrameSequencer::Step::Render);

			return true;
		}

		/// @brief 
		void World::Clear()
		{
			for (Scene* scene : _scenes)
			{
				delete scene;
			}
			_scenes.clear();

			_persistanteScene->Clear();
		}

		/// @brief 
		/// @return 
		Scene* World::CreateScene()
		{
			Scene* pScene = new Scene();

			_scenes.push_back(pScene);

			return pScene;
		}

		/// @brief 
		/// @param pScene 
		void World::DestroyScene(Scene* pScene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == pScene)
				{
					_scenes.erase(it);
					delete pScene;
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
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					// Todo message already
					return false;
				}
				++it;
			}

			_scenes.push_back(scene);
			return true;
		}

		/// @brief 
		/// @param scene 
		/// @return 
		bool World::RemoveScene(Scene* scene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					_scenes.erase(it);
					return true;
				}
				++it;
			}

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
			_editorNextFrame = false;
			//

			physics::Physics::GetInstance()->Update(0.016f);

			for (Scene* scene : _scenes)
			{
				scene->Update();
			}
		}

		/// @brief 
		void World::Draw()
		{
			if (_drawJobEnabled)
			{
				float size = 5.0f;

				window::Window* mainWindow = PlatformDisplayManager::GetInstance()->GetMainWindow();

				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY(0);
				viewport._size.SetX((float)mainWindow->GetWidth());
				viewport._size.SetY((float)mainWindow->GetHeight());

				float aspect = (float)mainWindow->GetWidth() / (float)mainWindow->GetHeight();

				Matrix4 projection = Matrix4::OrthogonalProjection(-size * aspect, size * aspect, -size, size, -1024, 1024);
				Matrix4 view = Matrix4();

				renderer::RenderQueue::GetInstance()->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

				Draw(renderer::RenderQueue::GetInstance());
			}
		}

		/// @brief 
		/// @param renderQueue 
		void World::Draw(renderer::RenderQueue* renderQueue)
		{
			for (Scene* scene : _scenes)
			{
				scene->Draw(renderQueue);
			}
			_persistanteScene->Draw(renderQueue);

			_debugDrawer.Draw(*renderQueue);
		}

		/// @brief 
		/// @return 
		const std::vector<Scene*>& World::GetScenes() const
		{
			return _scenes;
		}

		/// @brief 
		/// @param name 
		/// @return 
		std::weak_ptr<Entity> World::CreateEntity(const std::string_view& name)
		{
			return _persistanteScene->CreateEntity(name);
		}

		/// @brief 
		/// @param entity 
		void World::DestroyEntity(std::shared_ptr<Entity> entity)
		{
			for (Scene* scene : _scenes)
			{
				scene->DestroyEntity(entity);
			}
			_persistanteScene->DestroyEntity(entity);
		}

		/// @brief 
		/// @param entityId 
		/// @return 
		std::weak_ptr<Entity> World::FindEntity(Entity::Id entityId)
		{
			std::weak_ptr<Entity> entity;
			
			for (Scene* scene : _scenes)
			{
				entity = scene->FindEntity(entityId);
				if (entity.lock() != nullptr)
				{
					return entity;
				}
			}
			return _persistanteScene->FindEntity(entityId);
		}
	}
}
