#pragma once
#include <HodEngine/HodEngine.hpp>

#include <vector>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"

#include <functional>
#include <cassert>
#include <string>

namespace hod
{
	///@brief 
	class HOD_API ReflectionPropertyVariable : public ReflectionProperty
	{
		META_TYPE(ReflectionPropertyVariable, ReflectionProperty)

	public:

		enum class Type : uint8_t
		{
			Bool,
			Int8,
			Int16,
			Int32,
			Int64,
			UInt8,
			UInt16,
			UInt32,
			UInt64,
			Float32,
			Float64,
			String,
			Object,

			Count
		};
		inline static const char* _typeLabels[(uint8_t)Type::Count] = {
			"Bool",
			"Int8",
			"UInt8",
			"Int16",
			"UInt16",
			"Int32",
			"UInt32",
			"Int64",
			"UInt64",
			"Float32",
			"Float64",
			"String",
			"Object"
		};

	public:

												ReflectionPropertyVariable(Type type, uint32_t offset, const char* name, std::function<void(void*, void*)> setMethod, std::function<void*(const void*)> getMethod);
												ReflectionPropertyVariable(const ReflectionPropertyVariable& copy) = default;
												ReflectionPropertyVariable(ReflectionPropertyVariable&& move) = default;
												~ReflectionPropertyVariable() = default;

		ReflectionPropertyVariable&				operator = (const ReflectionPropertyVariable& copy) = default;
		ReflectionPropertyVariable&				operator = (ReflectionPropertyVariable&& move) = default;

	public:

		Type									GetType() const;

		template<typename _type_>
		_type_									GetValue(const void* instance) const;

		template<typename _type_>
		void									SetValue(void* instance, _type_ value) const;

		void									Copy(const void* sourceInstance, void* destinationInstance) const override;

		bool									CompareInstance(const void* left, const void* right) const;

	private:

		Type									_type;
		std::function<void(void*, void*)>		_setMethod;
		std::function<void*(const void*)>		_getMethod;
	};
}

#include "ReflectionPropertyVariable.inl"
