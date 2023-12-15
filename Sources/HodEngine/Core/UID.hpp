#pragma once

#include <string>
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	/// @brief 
	class UID final
	{
		REFLECTED_CLASS_NO_VIRTUAL(UID)
		
	public:

		static UID							INVALID_UID;

	public:

											UID() = default;
											UID(const UID&) = default;
											UID(UID&&) = default;
											~UID() = default;

		void								operator=(const UID& right);

		std::strong_ordering				operator<=>(const UID& other) const
		{
        	if (_high < other._high) return std::strong_ordering::less;
        	if (_high > other._high) return std::strong_ordering::greater;
        	if (_low < other._low) return std::strong_ordering::less;
        	if (_low > other._low) return std::strong_ordering::greater;
        	return std::strong_ordering::equal;
    	}

	public:

		std::string							ToString() const;
		bool								IsValid() const;

		bool								Compare(const UID& other) const;

	public:

		static UID							GenerateUID();
		static UID							FromString(const char* string);

	private:

		uint64_t							_low = 0;
		uint64_t							_high = 0;
	};

	bool									operator==(const UID& left, const UID& right);
	bool									operator!=(const UID& left, const UID& right);

	/*
	* 	bool								operator<(const UID& lhs, const UID& rhs)
	{
		return lhs._high <= rhs._high && lhs._low < rhs._low;
	}*/
}

#include "UID.inl"

