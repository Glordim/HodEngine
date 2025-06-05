#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/DebugDrawer.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <memory>
#include <map>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#include <HodEngine/Core/Time/SystemTime.hpp>

namespace hod
{
	class Vector2;
	struct Color;

	namespace physics
	{
		class World;
	}

	namespace game
	{
		class Scene;
		class RendererComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API World
		{
		// TODO remove
		public:

			static World*		CreateInstance() { _instance = new World(); return _instance; }
			static World*		GetInstance() { return _instance; }
			static void			DestroyInstance() { delete _instance; _instance = nullptr; }
			static World*		_instance;
		//

		public:

										World();
										~World();

			bool						Init();
			void						Clear();

// todo
			void						DisableDrawJob();

			void						SetEditorPlaying(bool editorPlaying);
			bool						GetEditorPlaying() const;

			void						SetEditorPaused(bool editorPaused);
			bool						GetEditorPaused() const;

			void						EditorNextFrame();
//

			World*						Clone();
			void						ProcessActication();

			void						Update();
			void						Draw();

			Scene*						CreateScene();
			void						DestroyScene(Scene* pScene);

			bool						AddScene(Scene* scene);
			bool						RemoveScene(Scene* scene);

			const Vector<Scene*>&	GetScenes() const;

			Entity*						CreateEntity(const std::string_view& name = "");
			void						DestroyEntity(Entity* entity);
			
			Entity*						FindEntity(uint64_t entityId);

//			bool						Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, physics::RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

			void						Draw(renderer::RenderQueue* renderQueue);

			// todo #ifndef retail ?
			void						DrawDebugLine(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);
			//

			physics::World*				GetPhysicsWorld() const;

		private:
// todo
			bool						_editorPlaying = false;
			bool						_editorPaused = false;
			bool						_editorNextFrame = false;
///
			MemberFunctionJob<World>	_updateJob;
			MemberFunctionJob<World>	_drawJob;
			bool						_drawJobEnabled = true;

			Vector<Scene*>			_scenes;
			Scene*						_persistanteScene = nullptr;

			DebugDrawer					_debugDrawer;

			SystemTime::TimeStamp		_lastUpdateTimestamp = SystemTime::INVALID_TIMESTAMP;

			bool						_jobInserted = false;

			float						_physicsUpdateTimestep = 1.0f / 120.0f;
			float						_accumulatedPhysicsTime = 0.0f;
			physics::World*				_physicsWorld = nullptr;
		};
	}
}
