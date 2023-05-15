#pragma once

#include <vector>

//#include <rapidjson/document.h>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
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
				class Array : public ReflectionProperty
				{

				public:

															Array(Variable::Type type, uint32_t offset, const char* name);
															Array(const Array& copy) = default;
															Array(Array&& move) = default;
															~Array() = default;

					Array&									operator = (const Array& copy) = default;
					Array&									operator = (Array&& move) = default;

				public:

					template<typename T>
					void									Serialize(const T& instance, Document::Node& node);

					template<typename T>
					void									Deserialize(T& instance, const Document::Node& node);

					//rapidjson::Value						Serialize(rapidjson::Document::AllocatorType& allocator) const override;

				private:

					Variable::Type							_type;
					uint32_t								_offset;
					const char*								_name;
				};
			}
		}
	}
}

#include "ReflectionPropertyArray.inl"
