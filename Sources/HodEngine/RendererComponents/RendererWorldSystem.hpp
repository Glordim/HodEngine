#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include <HodEngine/Game/WorldSystem.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

namespace hod::inline game
{
	class Scene;
}

namespace hod::inline renderer
{
	class MaterialInstance;
	class RenderView;

	/// @brief The render of a game::World: each frame on the main window (if the world is drawn there, see World::SetDrawnOnMainWindow), or
	/// in the render view of whoever draws it (the editor's viewports)
	class HOD_RENDERER_COMPONENTS_API RendererWorldSystem : public game::WorldSystem
	{
	public:
		explicit RendererWorldSystem(game::World& world);
		~RendererWorldSystem() override;

	public:
		static const game::WorldSystemType& GetType();

		/// @return nullptr if the world isn't rendered (the Renderer's module isn't registered)
		static RendererWorldSystem* Get(const game::World& world);

		/// @brief Its renderers, then its debug lines. Through its main camera when the world is playing (not paused), through the camera
		/// already set up in renderView otherwise (the editor's)
		void Draw(RenderView& renderView);

	private:
		void DrawOnMainWindow();
		void DrawScene(game::Scene& scene, RenderView& renderView);
		void DrawDebugLines(RenderView& renderView);

	private:
		MemberFunctionJob<RendererWorldSystem> _drawJob;
		const MaterialInstance*                _lineMaterial = nullptr;
	};
}
