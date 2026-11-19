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
	class ComponentInspectorBase;

	/// @brief The component inspectors by component type (see ComponentInspector), registered through GameEditorRegistrar::RegisterComponentInspector
	class HOD_GAME_EDITOR_API ComponentInspectorRegistry
	{
	public:
		/// @param inspector Owned by the registry, deleted right away if refused
		/// @return false (with an error) if the type has an inspector already: the first one stays
		static bool Register(const ReflectionDescriptor& descriptor, ComponentInspectorBase* inspector);
		static void Unregister(const ReflectionDescriptor& descriptor);

		/// @return The inspector of the type, or of its closest parent type that has one, nullptr if none
		template<typename _Component_>
		static ComponentInspectorBase* Find();
		static ComponentInspectorBase* Find(const ReflectionDescriptor& descriptor);

	private:
		static std::unordered_map<uint64_t, ComponentInspectorBase*> _inspectors;
	};

	/// @brief
	/// @tparam _Component_
	/// @return
	template<typename _Component_>
	ComponentInspectorBase* ComponentInspectorRegistry::Find()
	{
		return Find(_Component_::GetReflectionDescriptor());
	}
}
