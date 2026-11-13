#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp"

#include <HodEngine/Core/ModuleRegistration.hpp>

namespace hod::inline editor
{
	struct ViewportDebugViewType;

	/// @brief The registrar of GameEditor (see ModuleRegistration): what the modules add to the viewports of the entity based tabs (debug
	/// views, the game pointer when playing, the world's render), unregistered with the rest of what the module registered. In an editor
	/// module's startup (see HOD_STARTUP_MODULE)
	class HOD_GAME_EDITOR_API GameEditorRegistrar
	{
	public:
		explicit GameEditorRegistrar(ModuleRegistration& registration);

	public:
		/// @brief See ViewportDebugView
		/// @return false (with an error) if registered already
		bool RegisterViewportDebugView(const ViewportDebugViewType& type);

		/// @brief See ViewportGamePointer
		/// @return false (with an error) if registered already
		bool RegisterViewportGamePointer(ViewportGamePointer::Listener listener);

		/// @brief See ViewportWorldRenderer, one module only (the one rendering the worlds)
		/// @param setupPlayCamera nullptr: none, the viewports keep their default camera when playing
		void SetViewportWorldRenderer(ViewportWorldRenderer::DrawFunction draw, ViewportWorldRenderer::SetupPlayCameraFunction setupPlayCamera);

	private:
		ModuleRegistration& _registration;
	};
}
