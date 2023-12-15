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
	///@brief 
	class ReflectionHelper
	{
	public:

		template<typename _MemberVariable_>
		static ReflectionProperty*	AddProperty(ReflectionDescriptor* descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> = nullptr, std::function<void*(const void*)> = nullptr);
	};
}

#include "ReflectionHelper.inl"
