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
		_values.emplace_back(static_cast<uint64_t>(value), label);
	}

	template<typename __TYPE__>
	std::string_view EnumDescriptor::ToString(__TYPE__ value) const
	{
		for (uint32_t i = 0; i < _values.size(); ++i)
		{
			if (_values[i].first == static_cast<uint64_t>(value))
			{
				return _values[i].second;
			}
		}
		return std::string_view();
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	EnumDescriptor* EnumDescriptor::GenerateFromType()
	{
		static_assert(std::is_enum_v<__TYPE__>, "Requires enum type");

		EnumDescriptor* enumDescriptor = DefaultAllocator::GetInstance().New<EnumDescriptor>();

		enumDescriptor->_values.clear();
		/*
		for (uint64_t i = 0; i < static_cast<uint64_t>(__TYPE__::COUNT); ++i)
		{
		    enumDescriptor->_values.emplace_back(static_cast<uint64_t>(i), EnumTrait::ToString(static_cast<__TYPE__>(i)));
		}
		    */

		return enumDescriptor;
	}
}
