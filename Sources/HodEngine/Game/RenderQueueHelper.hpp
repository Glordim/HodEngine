#pragma once
#include <HodEngine/HodEngine.hpp>

namespace hod
{
	namespace game
	{
		class SceneComponent;
		class Scene;
	}

	namespace renderer
	{
		class RenderQueue;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_API RenderQueueHelper
	{
	public:

						RenderQueueHelper() = delete;
						RenderQueueHelper(const RenderQueueHelper&) = delete;
						RenderQueueHelper(RenderQueueHelper&&) = delete;
						~RenderQueueHelper() = delete;

		void			operator=(const RenderQueueHelper&) = delete;
		void			operator=(RenderQueueHelper&&) = delete;

	public:

		static void		AddSceneComponent(renderer::RenderQueue& renderQueue, game::SceneComponent* sceneComponent, bool recursive);
		static void		AddScenePhysicsDebug(renderer::RenderQueue& renderQueue, game::Scene* scene);
		static void		AddDebugLines(renderer::RenderQueue& renderQueue, game::Scene* scene);
	};
}
