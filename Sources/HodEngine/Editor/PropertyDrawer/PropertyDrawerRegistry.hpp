#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <unordered_map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class PropertyDrawerBase;

	/// @brief The property drawers of the types (see PropertyDrawer<T>), registered through EditorRegistrar::RegisterPropertyDrawer
	class HOD_EDITOR_API PropertyDrawerRegistry
	{
	public:
		/// @param drawer Owned by the registry, deleted right away if refused
		/// @return false (with an error) if the type has a drawer already: the first one stays
		static bool Register(const ReflectionDescriptor& descriptor, PropertyDrawerBase* drawer);
		static void Unregister(const ReflectionDescriptor& descriptor);

		/// @return The drawer of the type, or of its closest parent type having one; nullptr if none
		static PropertyDrawerBase* Find(const ReflectionDescriptor& descriptor);

	private:
		static std::unordered_map<uint64_t, PropertyDrawerBase*> _drawers;
	};
}
