#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include <cstdint>
#include <unordered_map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::editor
{
	class CustomComponentDrawer;

	class HOD_EDITOR_API CustomComponentDrawerRegistry
	{
	public:
		template<typename _Type_, typename _Drawer_>
		static void Register();
		static void Register(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer);

		template<typename _Type_>
		static void Unregister();
		static void Unregister(const ReflectionDescriptor& descriptor);

		template<typename _Type_>
		static CustomComponentDrawer* Find();
		static CustomComponentDrawer* Find(const ReflectionDescriptor& descriptor);

	private:

		static std::unordered_map<uint64_t, CustomComponentDrawer*> _drawers;
	};

	template<typename _Type_, typename _Drawer_>
	void CustomComponentDrawerRegistry::Register()
	{
		Register(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Type_>
	void CustomComponentDrawerRegistry::Unregister()
	{
		Unregister(_Type_::GetReflectionDescriptor());
	}

	template<typename _Type_>
	CustomComponentDrawer* CustomComponentDrawerRegistry::Find()
	{
		return Find(_Type_::GetReflectionDescriptor());
	}
}
