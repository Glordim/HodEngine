#pragma once

#include "HodEngine/Core/Type.h"

namespace hod
{
	namespace core
	{
		///@brief 
		class ReflectionTrait
		{
			BASE_META_TYPE(ReflectionTrait)

		public:

										ReflectionTrait();
										ReflectionTrait(const ReflectionTrait& copy) = default;
										ReflectionTrait(ReflectionTrait&& move) = default;
										~ReflectionTrait() = default;

			ReflectionTrait&			operator = (const ReflectionTrait& copy) = default;
			ReflectionTrait&			operator = (ReflectionTrait&& move) = default;
		};
	}
}
