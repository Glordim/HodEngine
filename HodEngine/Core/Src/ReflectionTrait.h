#pragma once

#include <rapidjson/document.h>

namespace HOD
{
	namespace CORE
	{
		///@brief 
		class ReflectionTrait
		{
		public:

								ReflectionTrait(const char* typeName);
								ReflectionTrait(const ReflectionTrait& copy) = default;
								ReflectionTrait(ReflectionTrait&& move) = default;
								~ReflectionTrait() = default;

			ReflectionTrait&	operator = (const ReflectionTrait& copy) = default;
			ReflectionTrait&	operator = (ReflectionTrait&& move) = default;

		public:

			virtual void		Serialize(rapidjson::Document& document) const = 0;

		private:

			const char*			_typeName;
		};
	}
}
