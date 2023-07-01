#pragma once

#include "HodEngine/Core/Reflection/ReflectionTrait.h"

namespace hod
{
	namespace core
	{
		///@brief 
		class ReflectionTraitFixedSizeArray : public ReflectionTrait
		{
			META_TYPE(ReflectionTraitFixedSizeArray)

		public:

											ReflectionTraitFixedSizeArray(uint32_t size);
											ReflectionTraitFixedSizeArray(const ReflectionTraitFixedSizeArray& copy) = default;
											ReflectionTraitFixedSizeArray(ReflectionTraitFixedSizeArray&& move) = default;
											~ReflectionTraitFixedSizeArray() = default;

			ReflectionTraitFixedSizeArray&	operator = (const ReflectionTraitFixedSizeArray& copy) = default;
			ReflectionTraitFixedSizeArray&	operator = (ReflectionTraitFixedSizeArray&& move) = default;

		public:

			uint32_t						GetFixedSize() const;

		protected:

			uint32_t						_fixedSize;
		};
	}
}
