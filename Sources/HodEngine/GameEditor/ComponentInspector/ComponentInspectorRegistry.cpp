#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentInspector/ComponentInspectorRegistry.hpp"
#include "HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline editor
{
	std::unordered_map<uint64_t, ComponentInspectorBase*> ComponentInspectorRegistry::_inspectors;

	/// @brief
	/// @param descriptor
	/// @param inspector
	/// @return
	bool ComponentInspectorRegistry::Register(const ReflectionDescriptor& descriptor, ComponentInspectorBase* inspector)
	{
		if (_inspectors.try_emplace(descriptor.GetType(), inspector).second == false)
		{
			OUTPUT_ERROR("ComponentInspectorRegistry: '{}' has an inspector already", descriptor.GetDisplayName());
			DefaultAllocator::GetInstance().Delete(inspector);
			return false;
		}
		return true;
	}

	/// @brief
	/// @param descriptor
	void ComponentInspectorRegistry::Unregister(const ReflectionDescriptor& descriptor)
	{
		auto it = _inspectors.find(descriptor.GetType());
		if (it != _inspectors.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_inspectors.erase(it);
		}
	}

	/// @brief
	/// @param descriptor
	/// @return
	ComponentInspectorBase* ComponentInspectorRegistry::Find(const ReflectionDescriptor& descriptor)
	{
		auto it = _inspectors.find(descriptor.GetType());
		if (it != _inspectors.end())
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
