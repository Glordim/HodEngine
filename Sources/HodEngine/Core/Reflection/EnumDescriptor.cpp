#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"

namespace hod
{
	/// @brief 
	/// @return 
	const Vector<std::pair<uint64_t, std::string>>& EnumDescriptor::GetValues() const
	{
		return _values;
	}
}
