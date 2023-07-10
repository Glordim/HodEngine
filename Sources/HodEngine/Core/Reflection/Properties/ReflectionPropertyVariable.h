#pragma once

#include <vector>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace Reflection
	{
		namespace Property
		{
			///@brief 
			class Variable : public ReflectionProperty
			{
				META_TYPE(Variable, ReflectionProperty)

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

														Variable(Type type, uint32_t offset, const char* name);
														Variable(const Variable& copy) = default;
														Variable(Variable&& move) = default;
														~Variable() = default;

				Variable&								operator = (const Variable& copy) = default;
				Variable&								operator = (Variable&& move) = default;

			public:

				void									Serialize(const void* instance, Document::Node& node) override;
				void									Deserialize(void* instance, const Document::Node& node) override;

				Type									GetType() const;
				const char*								GetName() const;

				template<typename _type_>
				_type_									GetValue(const void* instance) const;

				template<typename _type_>
				void									SetValue(void* instance, _type_ value);

			private:

				Type									_type;
				uint32_t								_offset;
				const char*								_name;
			};
		}
	}
}

#include "ReflectionPropertyVariable.inl"
