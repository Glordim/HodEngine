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

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	EnumDescriptor* EnumDescriptor::GenerateFromType()
	{
		static_assert(std::is_enum_v<__TYPE__>, "Requires enum type");

		EnumDescriptor* enumDescriptor = DefaultAllocator::GetInstance().New<EnumDescriptor>();

		enumDescriptor->_values.clear();
		for (__TYPE__ value : EnumTrait::GetValues<__TYPE__>())
		{
			enumDescriptor->_values.emplace_back(static_cast<uint64_t>(value), EnumTrait::ToString(value));
		}

		return enumDescriptor;
	}
}
