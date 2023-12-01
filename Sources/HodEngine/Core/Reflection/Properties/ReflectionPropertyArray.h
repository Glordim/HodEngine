#pragma once

#include <vector>
#include <functional>
#include <cassert>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace Reflection
	{
		namespace Property
		{
			///@brief 
			class Array : public ReflectionProperty
			{
				META_TYPE(Array, ReflectionProperty)

			public:

														Array(Variable::Type type, uint32_t offset, const char* name);
														Array(const Array& copy) = default;
														Array(Array&& move) = default;
														~Array() = default;

				Array&									operator = (const Array& copy) = default;
				Array&									operator = (Array&& move) = default;

			public:

				void									Serialize(const void* instance, Document::Node& node) override;
				void									Deserialize(void* instance, const Document::Node& node) override;

				Variable::Type							GetType() const;
				const char*								GetName() const;

				template<typename _type_>
				_type_									GetValue(const void* instance, uint32_t index) const;

				template<typename _type_>
				void									SetValue(void* instance, uint32_t index, _type_ value);

			private:

				static void*							GetElementAddress(void* instance, uint32_t index);

			private:

				Variable::Type							_type;
				uint32_t								_offset;
				const char*								_name;
				std::function<void*(void*, uint32_t)>	_getElementAddressFunction;
			};
		}
	}
}

#include "ReflectionPropertyArray.inl"
