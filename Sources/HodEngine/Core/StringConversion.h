#pragma once

#include <string_view>
#include <string>

namespace hod
{
	namespace StringConversion
	{
		bool	StringToInt8(const std::string_view& str, int8_t& value, uint8_t base = 10);
		bool	StringToUInt8(const std::string_view& str, uint8_t& value, uint8_t base = 10);
		bool	StringToInt16(const std::string_view& str, int16_t& value, uint8_t base = 10);
		bool	StringToUInt16(const std::string_view& str, uint16_t& value, uint8_t base = 10);
		bool	StringToInt32(const std::string_view& str, int32_t& value, uint8_t base = 10);
		bool	StringToUInt32(const std::string_view& str, uint32_t& value, uint8_t base = 10);
		bool	StringToInt64(const std::string_view& str, int64_t& value, uint8_t base = 10);
		bool	StringToUInt64(const std::string_view& str, uint64_t& value, uint8_t base = 10);
		bool	StringToFloat32(const std::string_view& str, float& value);
		bool	StringToFloat64(const std::string_view& str, double& value);

		template<typename ... Args>
		std::string StringFormat(const std::string_view& format, Args ... args )
		{
			int size = std::snprintf(nullptr, 0, format.data(), args ... ) + 1; // Extra space for '\0'
			if (size <= 0)
			{
				return std::string();
			}

			char buffer[4096] = {'\0'};

			std::string result;
			result.reserve(size);
			std::snprintf(buffer, size, format.data(), args ... );
			buffer[size - 1] = '\0';
			result = buffer;
			return result;
		}
	}
}
