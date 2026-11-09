#pragma once
#include "HodEngine/RendererComponents/Export.hpp"
#include "HodEngine/RendererComponents/DebugShapes.hpp"

#include <HodEngine/Game/WorldSystem.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>

namespace hod::inline game
{
	class Scene;
}

namespace hod::inline renderer
{
	class RenderView;

	/// @brief The render of a game::World: each frame on the main window (if the world is drawn there, see World::SetDrawnOnMainWindow), or
	/// in the render view of whoever draws it (the editor's viewports)
	class HOD_RENDERER_COMPONENTS_API RendererWorldSystem : public game::WorldSystem
	{
	public:
		/// @brief Rendering order of the world's debug shapes: over its content (Node2dComponent::GetZOrder), under the editor overlays
		static constexpr uint32_t DebugShapesOrder = 0xFFFFFFFF - 2;

	public:
		explicit RendererWorldSystem(game::World& world);
		~RendererWorldSystem() override;

	public:
		static const game::WorldSystemType& GetType();

		/// @return nullptr if the world isn't rendered (the Renderer's module isn't registered)
		static RendererWorldSystem* Get(const game::World& world);

		/// @brief The debug shapes of the world (see DebugShapes), nullptr if the world isn't rendered
		static DebugShapes* GetDebugShapes(const game::World& world);
		DebugShapes&        GetDebugShapes();

		/// @brief Its renderers, then its debug shapes over them. Through its main camera when the world is playing (not paused), through the camera
		/// already set up in renderView otherwise (the editor's)
		void Draw(RenderView& renderView);

		/// @brief Sets up renderView with its main camera (a builtin one without), as Draw does when playing
		void SetupPlayCamera(RenderView& renderView);

	protected:
		/// @brief Ages the debug shapes
		void OnUpdate(float deltaTime) override;

	private:
		void DrawOnMainWindow();
		void DrawScene(game::Scene& scene, RenderView& renderView);
		void DrawDebugShapes(RenderView& renderView);

	private:
		MemberFunctionJob<RendererWorldSystem> _drawJob;
		DebugShapes                            _debugShapes;
	};
}
