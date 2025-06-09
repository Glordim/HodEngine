#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod
{
	///@brief 
	class HOD_CORE_API ReflectionTraitSerializedByCallback : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitSerializedByCallback, ReflectionTrait)

	public:

													ReflectionTraitSerializedByCallback() = default;
													ReflectionTraitSerializedByCallback(const ReflectionTraitSerializedByCallback& copy) = default;
													ReflectionTraitSerializedByCallback(ReflectionTraitSerializedByCallback&& move) = default;
													~ReflectionTraitSerializedByCallback() = default;

		ReflectionTraitSerializedByCallback&		operator = (const ReflectionTraitSerializedByCallback& copy) = default;
		ReflectionTraitSerializedByCallback&		operator = (ReflectionTraitSerializedByCallback&& move) = default;
	};
}
