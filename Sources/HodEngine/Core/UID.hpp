#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	/// @brief 
	class HOD_CORE_API UID final
	{
		REFLECTED_CLASS_NO_VIRTUAL(UID)
		
	public:

		static UID							INVALID_UID;

	public:

											UID() = default;
											UID(uint64_t low, uint64_t high);
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

		bool								operator==(const UID& other) const = default;

	public:

		String								ToString() const;
		bool								IsValid() const;

		bool								Compare(const UID& other) const;

	public:

		static UID							GenerateUID();
		static UID							FromString(const char* string);

	private:

		uint64_t							_low = 0;
		uint64_t							_high = 0;
	};
}

#include "UID.inl"

