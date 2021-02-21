#pragma once

namespace HOD
{
	namespace GAME
	{
		class SceneComponent;
		class Scene;
	}

	class RenderQueue;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class RenderQueueHelper
	{
	public:

						RenderQueueHelper() = delete;
						RenderQueueHelper(const RenderQueueHelper&) = delete;
						RenderQueueHelper(RenderQueueHelper&&) = delete;
						~RenderQueueHelper() = delete;

		void			operator=(const RenderQueueHelper&) = delete;
		void			operator=(RenderQueueHelper&&) = delete;

	public:

		static void		AddSceneComponent(RenderQueue& renderQueue, GAME::SceneComponent* sceneComponent, bool recursive);
		static void		AddScenePhysicsDebug(RenderQueue& renderQueue, GAME::Scene* scene);
		static void		AddDebugLines(RenderQueue& renderQueue, GAME::Scene* scene);
	};
}
