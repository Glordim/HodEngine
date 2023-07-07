#pragma once

#include <string_view>

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
	}
}
