#pragma once

#include <vector>

#include <rapidjson/document.h>

#include <glm/vec2.hpp>

#include "../ReflectionProperty.h"

namespace hod
{
	namespace CORE
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
						Vector2f32,
						Count
					};
					inline static const char* _typeLabels[Type::Count] = { "Int8", "UInt8", "Int16", "UInt16", "Int32", "UInt32", "Int64", "UInt64", "Float32", "Float64", "Vector2f32" };

				public:

															Variable(Type type, uint32_t offset, const char* name);
															Variable(const Variable& copy) = default;
															Variable(Variable&& move) = default;
															~Variable() = default;

					Variable&								operator = (const Variable& copy) = default;
					Variable&								operator = (Variable&& move) = default;

				public:

					rapidjson::Value						Serialize(rapidjson::Document::AllocatorType& allocator) const override;

				private:

					Type									_type;
					uint32_t								_offset;
					const char*								_name;
				};
			}
		}
	}
}
