#pragma once

namespace HOD
{
	namespace GAME
	{
		class SceneComponent;
		class Scene;
	}

	namespace RENDERER
	{
		class RenderQueue;
	}

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

		static void		AddSceneComponent(RENDERER::RenderQueue& renderQueue, GAME::SceneComponent* sceneComponent, bool recursive);
		static void		AddScenePhysicsDebug(RENDERER::RenderQueue& renderQueue, GAME::Scene* scene);
		static void		AddDebugLines(RENDERER::RenderQueue& renderQueue, GAME::Scene* scene);
	};
}
