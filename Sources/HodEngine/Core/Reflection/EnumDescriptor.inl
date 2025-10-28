#include "HodEngine/Core/Reflection/EnumTrait.hpp"

namespace hod
{
	/// @brief
	/// @tparam __TYPE__
	/// @param value
	/// @param label
	template<typename __TYPE__>
	void EnumDescriptor::AddEnumValue(__TYPE__ value, const char* label)
	{
		_values.EmplaceBack(static_cast<uint64_t>(value), label);
	}

	template<typename __TYPE__>
	std::string_view EnumDescriptor::ToString(__TYPE__ value) const
	{
		for (uint32_t i = 0; i < _values.Size(); ++i)
		{
			if (_values[i].first == static_cast<uint64_t>(value))
			{
				return _values[i].second;
			}
		}
		return std::string_view();
	}
}
