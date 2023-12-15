#pragma once

#include "HodEngine/Core/Type.hpp"

namespace hod
{
	///@brief 
	class ReflectionTrait
	{
		BASE_META_TYPE(ReflectionTrait)

	public:

							ReflectionTrait() = default;
							ReflectionTrait(const ReflectionTrait& copy) = default;
							ReflectionTrait(ReflectionTrait&& move) = default;
		virtual				~ReflectionTrait() = default;

		ReflectionTrait&	operator = (const ReflectionTrait& copy) = default;
		ReflectionTrait&	operator = (ReflectionTrait&& move) = default;
	};
}
