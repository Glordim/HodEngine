#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <cstdint>

namespace hod::editor
{
	std::unordered_map<uint64_t, ComponentCustomEditor*> CustomComponentDrawerRegistry::_drawers;

	void CustomComponentDrawerRegistry::Register(const ReflectionDescriptor& descriptor, ComponentCustomEditor* drawer)
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

	ComponentCustomEditor* CustomComponentDrawerRegistry::Find(const ReflectionDescriptor& descriptor)
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
