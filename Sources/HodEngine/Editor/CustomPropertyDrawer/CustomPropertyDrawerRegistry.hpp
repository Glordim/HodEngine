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
	class CustomPropertyDrawer;

	class HOD_EDITOR_API CustomPropertyDrawerRegistry
	{
	public:
		template<typename _Type_, typename _Drawer_>
		static void Register();
		static void Register(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer);

		template<typename _Type_>
		static void Unregister();
		static void Unregister(const ReflectionDescriptor& descriptor);

		template<typename _Type_>
		static CustomPropertyDrawer* Find();
		static CustomPropertyDrawer* Find(const ReflectionDescriptor& descriptor);

	private:

		static std::unordered_map<uint64_t, CustomPropertyDrawer*> _drawers;
	};

	template<typename _Type_, typename _Drawer_>
	void CustomPropertyDrawerRegistry::Register()
	{
		Register(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Type_>
	void CustomPropertyDrawerRegistry::Unregister()
	{
		Unregister(_Type_::GetReflectionDescriptor());
	}

	template<typename _Type_>
	CustomPropertyDrawer* CustomPropertyDrawerRegistry::Find()
	{
		return Find(_Type_::GetReflectionDescriptor());
	}
}
