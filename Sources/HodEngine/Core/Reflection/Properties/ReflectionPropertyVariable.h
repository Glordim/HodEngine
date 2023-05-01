#pragma once

#include <vector>

//#include <rapidjson/document.h>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				///@brief 
				class Variable : public ReflectionProperty
				{
				public:

					enum Type
					{
						Bool,
						Int8,
						UInt8,
						Int16,
						UInt16,
						Int32,
						UInt32,
						Int64,
						UInt64,
						Float32,
						Float64,
						Count
					};
					inline static const char* _typeLabels[Type::Count] = { "Bool", "Int8", "UInt8", "Int16", "UInt16", "Int32", "UInt32", "Int64", "UInt64", "Float32", "Float64" };

				public:

															Variable(Type type, uint32_t offset, const char* name);
															Variable(const Variable& copy) = default;
															Variable(Variable&& move) = default;
															~Variable() = default;

					Variable&								operator = (const Variable& copy) = default;
					Variable&								operator = (Variable&& move) = default;

				public:

					template<typename T>
					void									Serialize(const T& instance, Document::Node& node);

					template<typename T>
					void									Deserialize(T& instance, const Document::Node& node);

					//rapidjson::Value						Serialize(rapidjson::Document::AllocatorType& allocator) const override;

				private:

					Type									_type;
					uint32_t								_offset;
					const char*								_name;
				};
			}
		}
	}
}

#include "ReflectionPropertyVariable.inl"
