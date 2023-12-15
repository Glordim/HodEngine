#pragma once

#include <vector>
#include <functional>
#include <cassert>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

namespace hod
{
	///@brief 
	class ReflectionPropertyArray : public ReflectionProperty
	{
		META_TYPE(ReflectionPropertyArray, ReflectionProperty)

	public:

												ReflectionPropertyArray(ReflectionPropertyVariable::Type type, uint32_t offset, const char* name);
												ReflectionPropertyArray(const ReflectionPropertyArray& copy) = default;
												ReflectionPropertyArray(ReflectionPropertyArray&& move) = default;
												~ReflectionPropertyArray() = default;

		ReflectionPropertyArray&				operator = (const ReflectionPropertyArray& copy) = default;
		ReflectionPropertyArray&				operator = (ReflectionPropertyArray&& move) = default;

	public:

		ReflectionPropertyVariable::Type		GetType() const;

		template<typename _type_>
		_type_									GetValue(const void* instance, uint32_t index) const;

		template<typename _type_>
		void									SetValue(void* instance, uint32_t index, _type_ value);

	private:

		static void*							GetElementAddress(void* instance, uint32_t index);

	private:

		ReflectionPropertyVariable::Type		_type;
		std::function<void*(void*, uint32_t)>	_getElementAddressFunction;
	};
}

#include "ReflectionPropertyArray.inl"
