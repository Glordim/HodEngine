#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <cstdint>

namespace hod::inline editor
{
	std::unordered_map<uint64_t, CustomPropertyDrawer*> CustomPropertyDrawerRegistry::_drawers;

	bool CustomPropertyDrawerRegistry::Register(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer)
	{
		if (_drawers.try_emplace(descriptor.GetType(), drawer).second == false)
		{
			OUTPUT_ERROR("CustomPropertyDrawerRegistry: '{}' has a drawer already", descriptor.GetDisplayName());
			DefaultAllocator::GetInstance().Delete(drawer);
			return false;
		}
		return true;
	}

	void CustomPropertyDrawerRegistry::Unregister(const ReflectionDescriptor& descriptor)
	{
		uint64_t type = descriptor.GetType();
		auto it = _drawers.find(type);
		if (it != _drawers.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_drawers.erase(it);
		}
	}

	CustomPropertyDrawer* CustomPropertyDrawerRegistry::Find(const ReflectionDescriptor& descriptor)
	{
		uint64_t type = descriptor.GetType();
		auto it = _drawers.find(type);
		if (it != _drawers.end())
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
