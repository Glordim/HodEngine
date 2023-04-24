#pragma once

#include <rapidjson/document.h>

namespace hod
{
	namespace core
	{
		///@brief 
		class ReflectionTrait
		{
		public:

										ReflectionTrait(const char* typeName);
										ReflectionTrait(const ReflectionTrait& copy) = default;
										ReflectionTrait(ReflectionTrait&& move) = default;
										~ReflectionTrait() = default;

			ReflectionTrait&			operator = (const ReflectionTrait& copy) = default;
			ReflectionTrait&			operator = (ReflectionTrait&& move) = default;

		public:

			virtual rapidjson::Value	Serialize(rapidjson::Document::AllocatorType& allocator) const = 0;

		protected:

			const char*					_typeName;
		};
	}
}
