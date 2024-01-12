#pragma once

#include <type_traits>
#include <string_view>
#include <functional>

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

namespace hod
{
	// Structure générique
	template <typename T>
	struct IsArray : std::false_type {};

	// Spécialisation partielle pour std::array
	template <typename T, size_t size>
	struct IsArray<std::array<T, size>> : std::true_type {};

	// Structure générique
	template <typename T>
	struct IsVector : std::false_type {};

	// Spécialisation partielle pour std::vector
	template <typename T, typename Allocator>
	struct IsVector<std::vector<T, Allocator>> : std::true_type {};
	
	///@brief 
	class ReflectionHelper
	{
	public:

		template<typename _MemberVariable_>
		static ReflectionProperty*	AddProperty(ReflectionDescriptor* descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> = nullptr, std::function<void*(const void*)> = nullptr);
	};
}

#include "ReflectionHelper.inl"
