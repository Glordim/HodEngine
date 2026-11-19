#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentEnabledIf.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param component
	/// @param owner
	/// @return
	bool ReflectionTraitVectorComponentEnabledIf::IsEnabled(uint32_t component, const void* owner) const
	{
		if (component >= MaxComponentCount || _predicates[component] == nullptr)
		{
			return true;
		}
		return _predicates[component](owner);
	}

	/// @brief
	/// @param component
	/// @return
	const char* ReflectionTraitVectorComponentEnabledIf::GetReason(uint32_t component) const
	{
		if (component >= MaxComponentCount || _reasons[component].Empty())
		{
			return nullptr;
		}
		return _reasons[component].CStr();
	}
}
