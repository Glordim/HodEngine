#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <memory>
#include <map>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

#include <HodEngine/Core/Time/SystemTime.hpp>

namespace hod::inline math { struct Color; class Vector2; }

namespace hod::inline game
{
	class Scene;
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

		/// @brief Drawn each frame on the main window by what renders the worlds (the Renderer's module, see RendererWorldSystem), the
		/// default; false for the worlds drawn by their owner (the editor's tabs, in their viewports)
		void						SetDrawnOnMainWindow(bool drawnOnMainWindow);
		bool						IsDrawnOnMainWindow() const;

// todo
		void						SetEditorPlaying(bool editorPlaying);
		bool						GetEditorPlaying() const;

		void						SetEditorPaused(bool editorPaused);
		bool						GetEditorPaused() const;

		void						EditorNextFrame();
//

		World*						Clone();
		void						ProcessActication();

		void						Update();

		Scene*						CreateScene();
		void						DestroyScene(Scene* pScene);

		bool						AddScene(Scene* scene);
		bool						RemoveScene(Scene* scene);

		const Vector<Scene*>&		GetScenes() const;

		/// @brief Of the entities created in the world itself (see CreateEntity), never null
		Scene*						GetPersistentScene() const;

		Entity*						CreateEntity(const std::string_view& name = "");
		void						DestroyEntity(Entity* entity);
		
		Entity*						FindEntity(uint64_t entityId);

//			bool						Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, RaycastResult& result, bool drawDebug, const Color& debugColor, float debugDuration);

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
		bool						_drawnOnMainWindow = true;

		Vector<Scene*>				_scenes;
		Scene*						_persistanteScene = nullptr;

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
