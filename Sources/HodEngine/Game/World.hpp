#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/DebugDrawer.hpp"

#include <vector>
#include <memory>
#include <map>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

namespace hod
{
	class Vector2;
	struct Color;

	namespace game
	{
		class Scene;
		class RendererComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API World
		{
			_Singleton(World)

		public:

			bool						Init();
			void						Clear();

#if defined(HOD_EDITOR)
			void						DisableDrawJob();

			void						SetEditorPlaying(bool editorPlaying);
			bool						GetEditorPlaying() const;

			void						SetEditorPaused(bool editorPaused);
			bool						GetEditorPaused() const;
#endif

			void						Update();
			void						Draw();

			Scene*						CreateScene();
			void						DestroyScene(Scene* pScene);

			bool						AddScene(Scene* scene);
			bool						RemoveScene(Scene* scene);

			const std::vector<Scene*>&	GetScenes() const;

			std::weak_ptr<Entity>		CreateEntity(const std::string_view& name = "");
			void						DestroyEntity(std::shared_ptr<Entity> entity);
			
			std::weak_ptr<Entity>		FindEntity(Entity::Id entityId);

//			bool						Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, physics::RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

			void						Draw(renderer::RenderQueue* renderQueue);
			void						DrawPicking(renderer::RenderQueue* renderQueue, std::map<uint32_t, std::shared_ptr<RendererComponent>>& colorIdToRendererComponentMap);

			// todo #ifndef retail ?
			void						DrawDebugLine(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);
			//

		protected:

										~World();

		private:
#if defined(HOD_EDITOR)
			bool						_editorPlaying = false;
			bool						_editorPaused = false;

			std::vector<Scene*>			_backupedScenes;
#endif
			MemberFunctionJob<World>	_updateJob;
			MemberFunctionJob<World>	_drawJob;
			bool						_drawJobEnabled = true;

			std::vector<Scene*>			_scenes;
			Scene*						_persistanteScene = nullptr;

			DebugDrawer					_debugDrawer;
		};
	}
}
