#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <cstdint>
#include <unordered_map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class ComponentGizmoBase;

	/// @brief The component gizmos by component type (see ComponentGizmo), registered through GameEditorRegistrar::RegisterComponentGizmo
	class HOD_GAME_EDITOR_API ComponentGizmoRegistry
	{
	public:
		/// @param gizmo Owned by the registry, deleted right away if refused
		/// @return false (with an error) if the type has a gizmo already: the first one stays
		static bool Register(const ReflectionDescriptor& descriptor, ComponentGizmoBase* gizmo);
		static void Unregister(const ReflectionDescriptor& descriptor);

		/// @return The gizmo of the type, or of its closest parent type that has one, nullptr if none
		template<typename _Component_>
		static ComponentGizmoBase* Find();
		static ComponentGizmoBase* Find(const ReflectionDescriptor& descriptor);

	private:
		static std::unordered_map<uint64_t, ComponentGizmoBase*> _gizmos;
	};

	/// @brief
	/// @tparam _Component_
	/// @return
	template<typename _Component_>
	ComponentGizmoBase* ComponentGizmoRegistry::Find()
	{
		return Find(_Component_::GetReflectionDescriptor());
	}
}
