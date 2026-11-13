#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include <cstdint>
#include <unordered_map>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class CustomComponentDrawer;

	class HOD_EDITOR_API CustomComponentDrawerRegistry
	{
	public:
		/// @param drawer Owned by the registry, deleted right away if refused
		/// @return false (with an error) if the type has a drawer already: the first one stays
		template<typename _Type_, typename _Drawer_>
		static bool Register();
		static bool Register(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer);

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
	bool CustomComponentDrawerRegistry::Register()
	{
		return Register(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
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
