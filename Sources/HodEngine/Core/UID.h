#pragma once

#include <string>

namespace hod
{
	/// @brief 
	class UID final
	{
	public:

		static UID							INVALID_UID;

	public:

		UID() = default;
		UID(const UID& copy) = default;
		~UID() = default;

		void								operator=(const UID& right);

		bool								operator<(const UID& rhs) const
		{
			return _high <= rhs._high && _low < rhs._low;
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
