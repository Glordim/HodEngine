#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentLabels.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param x
	/// @param y
	/// @param z
	/// @param w
	ReflectionTraitVectorComponentLabels::ReflectionTraitVectorComponentLabels(std::string_view x, std::string_view y, std::string_view z, std::string_view w)
	: ReflectionTrait()
	, _labels {String(x), String(y), String(z), String(w)}
	{
	}

	/// @brief
	/// @param component
	/// @return
	const char* ReflectionTraitVectorComponentLabels::GetLabel(uint32_t component) const
	{
		if (component >= MaxComponentCount || _labels[component].Empty())
		{
			return nullptr;
		}
		return _labels[component].CStr();
	}
}
