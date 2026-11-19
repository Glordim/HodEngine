#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Game/Component.hpp>

namespace hod::inline editor
{
	class GizmoViewport;

	template<typename _Component_>
	class ComponentGizmo;

	/// @brief What ComponentGizmo<T> doesn't need its type for: called by the viewports (ViewportWindow) with a component whose type is known at
	/// runtime only. Not derived from directly: see ComponentGizmo<T>
	class HOD_GAME_EDITOR_API ComponentGizmoBase
	{
	public:
		virtual ~ComponentGizmoBase() = default;

		ComponentGizmoBase(const ComponentGizmoBase&) = delete;
		ComponentGizmoBase(ComponentGizmoBase&&) = delete;

		void operator=(const ComponentGizmoBase&) = delete;
		void operator=(ComponentGizmoBase&&) = delete;

	public:
		/// @brief component: of the type the gizmo is registered for (or deriving from it)
		/// @return true if the component changed (a step of the tab's undo history)
		virtual bool Draw(Component& component, GizmoViewport& viewport, bool active) = 0;

	private:
		template<typename _Component_>
		friend class ComponentGizmo;

		ComponentGizmoBase() = default;
	};

	/// @brief What a component of type _Component_ (or deriving from it) shows and lets manipulate in the editor viewports: shapes drawn with
	/// ShapeDrawer, handles (Handles). Registered by an editor module (GameEditorRegistrar::RegisterComponentGizmo), one instance for every
	/// component of the type
	/// @tparam _Component_ The component type
	template<typename _Component_>
	class ComponentGizmo : public ComponentGizmoBase
	{
	public:
		using ComponentType = _Component_;

	protected:
		ComponentGizmo() = default;

		/// @brief Each frame, for each enabled component of the type in the viewport
		/// @param active Its entity is the active one of the selection: the one whose handles are shown
		/// @return true if the component changed (a step of the tab's undo history)
		virtual bool OnDraw(_Component_& component, GizmoViewport& viewport, bool active) = 0;

	private:
		bool Draw(Component& component, GizmoViewport& viewport, bool active) final;
	};

	/// @brief
	/// @tparam _Component_
	/// @param component
	/// @param viewport
	/// @param active
	/// @return
	template<typename _Component_>
	bool ComponentGizmo<_Component_>::Draw(Component& component, GizmoViewport& viewport, bool active)
	{
		return OnDraw(static_cast<_Component_&>(component), viewport, active);
	}
}
