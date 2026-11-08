#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/DebugDrawer.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <memory>
#include <map>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

#include <HodEngine/Core/Time/SystemTime.hpp>

namespace hod::inline math { struct Color; class Vector2; }
namespace hod::inline renderer { class RenderView; }

namespace hod::inline game
{
	class Scene;
	class RendererComponent;
	class WorldSystem;
	struct WorldSystemType;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_GAME_API World
	{
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

		const Vector<Scene*>&		GetScenes() const;

		Entity*						CreateEntity(const std::string_view& name = "");
		void						DestroyEntity(Entity* entity);
		
		Entity*						FindEntity(uint64_t entityId);

//			bool						Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

		void						Draw(RenderView& renderView);

		// todo #ifndef retail ?
		void						DrawDebugLine(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);
		//

		/// @brief The instance of a WorldSystemType registered by a module, nullptr if the module isn't there
		WorldSystem*				FindSystem(const WorldSystemType& type) const;

		/// @brief Of the scenes' FixedUpdate and the systems' OnFixedUpdate
		float						GetFixedTimestep() const;

	private:
		friend class WorldSystemRegistry;

		void						AddSystem(const WorldSystemType& type);
		void						RemoveSystem(const WorldSystemType& type);

	private:
// todo
		bool						_editorPlaying = false;
		bool						_editorPaused = false;
		bool						_editorNextFrame = false;
///
		MemberFunctionJob<World>	_updateJob;
		MemberFunctionJob<World>	_drawJob;
		bool						_drawJobEnabled = true;

		Vector<Scene*>				_scenes;
		Scene*						_persistanteScene = nullptr;

		DebugDrawer					_debugDrawer;

		SystemTime::TimeStamp		_lastUpdateTimestamp = SystemTime::INVALID_TIMESTAMP;

		bool						_jobInserted = false;

		float						_fixedTimestep = 1.0f / 120.0f;
		float						_accumulatedFixedTime = 0.0f;

		struct SystemInstance
		{
			const WorldSystemType*	_type = nullptr;
			WorldSystem*			_system = nullptr;
		};
		Vector<SystemInstance>		_systems; // In the WorldSystemRegistry order
		bool						_systemsCreated = false;
	};
}
