#pragma once

#include <type_traits>
#include <string_view>
#include <functional>

#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"

namespace hod
{
	///@brief 
	class ReflectionHelper
	{
	public:

		template<typename _MemberVariable_>
		static ReflectionProperty*	AddProperty(ReflectionDescriptor* descriptor, const std::string_view& name, uint32_t offset, std::function<void(void*, void*)> = nullptr);
	};
}

#include "ReflectionHelper.inl"
