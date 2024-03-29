#pragma once

#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

namespace hod
{
	namespace game
	{
		class Scene;
		class PhysicsDebugDrawer;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class World
		{
			_Singleton(World)

		public:

			bool				Init();
			void				Clear();

#if defined(HOD_EDITOR)
			void				SetEditorPlaying(bool editorPlaying);
			bool				GetEditorPlaying() const;

			void				SetEditorPaused(bool editorPaused);
			bool				GetEditorPaused() const;
#endif

			void				Update();

			Scene*				CreateScene();
			void				DestroyScene(Scene* pScene);

			bool				AddScene(Scene* scene);
			bool				RemoveScene(Scene* scene);

			const std::vector<Scene*>&	GetScenes() const;

			std::weak_ptr<Entity>			CreateEntity(const std::string_view& name = "");
			void							DestroyEntity(std::shared_ptr<Entity> entity);
			const std::unordered_map<Entity::Id, std::shared_ptr<Entity>>& GetEntities() const;

			std::weak_ptr<Entity> FindEntity(Entity::Id entityId);

			Event<std::weak_ptr<Entity>>&	GetAddEntityEvent() { return _addEntityEvent; }
			Event<std::weak_ptr<Entity>>&	GetRemoveEntityEvent() { return _removeEntityEvent; }
			Event<std::weak_ptr<Entity>>&	GetRenameEntityEvent() { return _renameEntityEvent; }

//			bool							Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, physics::RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

			bool							SaveToDocument(Document::Node& documentNode);
			bool							LoadFromDocument(const Document::Node& documentNode);

			void							EnablePhysicsDebugDrawer(bool enabled);
			bool							IsPhysicsDebugDrawerEnabled(bool enabled) const;

			void							Draw(renderer::RenderQueue* renderQueue);

		protected:

								~World();

		private:
#if defined(HOD_EDITOR)
			bool						_editorPlaying = false;
			bool						_editorPaused = false;
#endif
			MemberFunctionJob<World>	_updateJob;

			std::vector<Scene*>	_scenes;
			std::unordered_map<Entity::Id, std::shared_ptr<Entity>>	_entities;

			Event<std::weak_ptr<Entity>>	_addEntityEvent;
			Event<std::weak_ptr<Entity>>	_removeEntityEvent;
			Event<std::weak_ptr<Entity>>	_renameEntityEvent;

			PhysicsDebugDrawer*				_physicsDebugDrawer = nullptr;
		};
	}
}
