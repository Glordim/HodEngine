#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <utility>

#include "HodEngine/Core/Reflection/EnumTrait.hpp"

namespace hod::inline core
{
	///@brief
	class HOD_CORE_API EnumDescriptor
	{
	public:
		EnumDescriptor() = default;
		EnumDescriptor(const EnumDescriptor& copy) = default;
		EnumDescriptor(EnumDescriptor&& move) = default;
		~EnumDescriptor() = default;

		EnumDescriptor& operator=(const EnumDescriptor& copy) = default;
		EnumDescriptor& operator=(EnumDescriptor&& move) = default;

	public:
		const Vector<std::pair<uint64_t, String>>& GetValues() const;

		template<typename __TYPE__>
		std::string_view ToString(__TYPE__ value) const;

		template<typename __TYPE__>
		void AddEnumValue(__TYPE__ value, const char* label);

	private:
		Vector<std::pair<uint64_t, String>> _values;
	};

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
