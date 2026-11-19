#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp"
#include "HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/ModuleRegistration.hpp>

#include <type_traits>

namespace hod::inline editor
{
	struct ViewportDebugViewType;

	/// @brief The registrar of GameEditor (see ModuleRegistration): what the modules add to the entity based tabs (component gizmos and
	/// inspectors; to the viewports: debug views, the game pointer when playing, the world's render), unregistered with the rest of what the module registered. In an editor
	/// module's startup (see HOD_STARTUP_MODULE)
	class HOD_GAME_EDITOR_API GameEditorRegistrar
	{
	public:
		explicit GameEditorRegistrar(ModuleRegistration& registration);

	public:
		/// @brief An instance of _Gizmo_ (deriving from ComponentGizmo<T>) for the components of type T and the ones deriving from it
		/// @return false (with an error) if T has a gizmo already (from another module)
		template<typename _Gizmo_>
		bool RegisterComponentGizmo();
		/// @param gizmo Owned by the registration, deleted right away if refused
		bool RegisterComponentGizmo(const ReflectionDescriptor& descriptor, ComponentGizmoBase* gizmo);

		/// @brief An instance of _Inspector_ (deriving from ComponentInspector<T>) for the components of type T and the ones deriving from it
		/// @return false (with an error) if T has an inspector already (from another module)
		template<typename _Inspector_>
		bool RegisterComponentInspector();
		/// @param inspector Owned by the registration, deleted right away if refused
		bool RegisterComponentInspector(const ReflectionDescriptor& descriptor, ComponentInspectorBase* inspector);

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

	/// @brief
	/// @tparam _Gizmo_
	/// @return
	template<typename _Gizmo_>
	bool GameEditorRegistrar::RegisterComponentGizmo()
	{
		using ComponentType = typename _Gizmo_::ComponentType;
		static_assert(std::is_base_of_v<ComponentGizmo<ComponentType>, _Gizmo_>, "a gizmo derives from ComponentGizmo<T>");
		return RegisterComponentGizmo(ComponentType::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Gizmo_>());
	}

	/// @brief
	/// @tparam _Inspector_
	/// @return
	template<typename _Inspector_>
	bool GameEditorRegistrar::RegisterComponentInspector()
	{
		using ComponentType = typename _Inspector_::ComponentType;
		static_assert(std::is_base_of_v<ComponentInspector<ComponentType>, _Inspector_>, "an inspector derives from ComponentInspector<T>");
		return RegisterComponentInspector(ComponentType::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Inspector_>());
	}
}
