#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <cstdint>

namespace hod::inline editor
{
	std::unordered_map<uint64_t, CustomComponentDrawer*> CustomComponentDrawerRegistry::_drawers;

	void CustomComponentDrawerRegistry::Register(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer)
	{
		uint64_t type = descriptor.GetType();
		if (_drawers.find(type) == _drawers.end())
		{
			_drawers[type] = drawer;
		}
		else
		{
			DefaultAllocator::GetInstance().Delete(drawer);
		}
	}

	void CustomComponentDrawerRegistry::Unregister(const ReflectionDescriptor& descriptor)
	{
		uint64_t type = descriptor.GetType();
		auto it = _drawers.find(type);
		if (it != _drawers.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_drawers.erase(it);
		}
	}

	CustomComponentDrawer* CustomComponentDrawerRegistry::Find(const ReflectionDescriptor& descriptor)
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
