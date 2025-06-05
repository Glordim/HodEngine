#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <utility>

namespace hod
{
	///@brief
	class HOD_CORE_API EnumDescriptor
	{
	public:

										EnumDescriptor() = default;
										EnumDescriptor(const EnumDescriptor& copy) = default;
										EnumDescriptor(EnumDescriptor&& move) = default;
										~EnumDescriptor() = default;

		EnumDescriptor&					operator = (const EnumDescriptor& copy) = default;
		EnumDescriptor&					operator = (EnumDescriptor&& move) = default;

	public:

		const Vector<std::pair<uint64_t, std::string>>&	GetValues() const;

		template<typename __TYPE__>
		void AddEnumValue(__TYPE__ value, const char* label);

		template<typename __TYPE__>
		static EnumDescriptor* GenerateFromType();

	private:

		Vector<std::pair<uint64_t, std::string>> _values;
	};
}

#include "HodEngine/Core/Reflection/EnumDescriptor.inl"
