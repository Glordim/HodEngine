#pragma once

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

		protected:

			const char*					_typeName;
		};
	}
}
