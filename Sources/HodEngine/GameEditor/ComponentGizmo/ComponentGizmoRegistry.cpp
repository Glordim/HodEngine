#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmoRegistry.hpp"
#include "HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline editor
{
	std::unordered_map<uint64_t, ComponentGizmoBase*> ComponentGizmoRegistry::_gizmos;

	/// @brief
	/// @param descriptor
	/// @param gizmo
	/// @return
	bool ComponentGizmoRegistry::Register(const ReflectionDescriptor& descriptor, ComponentGizmoBase* gizmo)
	{
		if (_gizmos.try_emplace(descriptor.GetType(), gizmo).second == false)
		{
			OUTPUT_ERROR("ComponentGizmoRegistry: '{}' has a gizmo already", descriptor.GetDisplayName());
			DefaultAllocator::GetInstance().Delete(gizmo);
			return false;
		}
		return true;
	}

	/// @brief
	/// @param descriptor
	void ComponentGizmoRegistry::Unregister(const ReflectionDescriptor& descriptor)
	{
		auto it = _gizmos.find(descriptor.GetType());
		if (it != _gizmos.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_gizmos.erase(it);
		}
	}

	/// @brief
	/// @param descriptor
	/// @return
	ComponentGizmoBase* ComponentGizmoRegistry::Find(const ReflectionDescriptor& descriptor)
	{
		auto it = _gizmos.find(descriptor.GetType());
		if (it != _gizmos.end())
		{
			return it->second;
		}

		const ReflectionDescriptor* parentDescriptor = descriptor.GetParent();
		if (parentDescriptor != nullptr)
		{
			return Find(*parentDescriptor);
		}
		return nullptr;
	}
}
