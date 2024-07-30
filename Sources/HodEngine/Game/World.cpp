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
#include <HodEngine/Game/PhysicsDebugDrawer.hpp>

namespace hod
{
	namespace game
	{
#if defined(HOD_EDITOR)
		void World::SetEditorPlaying(bool editorPlaying)
		{
			if (_editorPlaying != editorPlaying)
			{
				_editorPlaying = editorPlaying;
				if (editorPlaying == true)
				{
					_persistanteScene->Clear();
					for (Scene* scene : _scenes)
					{
						Scene* clone = scene->Clone();
						_backupedScenes.push_back(clone);
					}

					for (Scene* scene : _scenes)
					{
						scene->Awake();
					}

					for (Scene* scene : _scenes)
					{
						scene->Awake();
					}
				}
				else
				{
					Clear();

					for (Scene* scene : _backupedScenes)
					{
						_scenes.push_back(scene);
					}
					_backupedScenes.clear();
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
#endif

		/// @brief 
		/// @param  
		_SingletonConstructor(World)
		: _updateJob(this, &World::Update, JobQueue::Queue::FramedNormalPriority)
		, _persistanteScene(new Scene())
		{
		}

		/// @brief 
		World::~World()
		{
			FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::Logic);

			Clear();

			delete _persistanteScene;
		}

		/// @brief 
		/// @return 
		bool World::Init()
		{
			FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::Logic);

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
			}

			// Todo message not found
			return false;
		}

		/// @brief 
		void World::Update()
		{
#if defined(HOD_EDITOR)
			if (_editorPlaying == false || _editorPaused == true)
			{
				return;
			}
#endif

			physics::Physics::GetInstance()->Update(0.016f);

			for (Scene* scene : _scenes)
			{
				scene->Update();
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

			if (_physicsDebugDrawer != nullptr)
			{
				_physicsDebugDrawer->PushToRenderQueue(*renderQueue);
			}
		}

		/// @brief 
		/// @param renderQueue 
		void World::DrawPicking(renderer::RenderQueue* renderQueue, std::map<uint32_t, std::shared_ptr<RendererComponent>>& colorIdToRendererComponentMap)
		{
			uint32_t id = 0;
			for (Scene* scene : _scenes)
			{
				scene->DrawPicking(renderQueue, colorIdToRendererComponentMap, id);
			}
			_persistanteScene->DrawPicking(renderQueue, colorIdToRendererComponentMap, id);
		}


		/// @brief 
		/// @param enabled 
		void World::EnablePhysicsDebugDrawer(bool enabled)
		{
			if (enabled == true)
			{
				if (_physicsDebugDrawer == nullptr)
				{
					_physicsDebugDrawer = new PhysicsDebugDrawer();
					physics::Physics::GetInstance()->SetDebugDrawer(_physicsDebugDrawer);
				}
			}
			else
			{
				if (_physicsDebugDrawer != nullptr)
				{
					physics::Physics::GetInstance()->SetDebugDrawer(nullptr);
					delete _physicsDebugDrawer;
					_physicsDebugDrawer = nullptr;
				}
			}
		}

		/// @brief 
		/// @param enabled 
		/// @return 
		bool World::IsPhysicsDebugDrawerEnabled(bool enabled) const
		{
			return (_physicsDebugDrawer != nullptr);
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
