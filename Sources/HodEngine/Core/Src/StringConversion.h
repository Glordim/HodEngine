#pragma once

#include <string>
#include <vector>

namespace HOD
{
	namespace CORE
	{
		namespace StringConversion
		{
			bool	StringToInt8(const char* str, int8_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToInt8(const std::string& str, int8_t& value, uint8_t base = 10);

			bool	StringToUInt8(const char* str, uint8_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToUInt8(const std::string& str, uint8_t& value, uint8_t base = 10);

			bool	StringToInt16(const char* str, int16_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToInt16(const std::string& str, int16_t& value, uint8_t base = 10);

			bool	StringToUInt16(const char* str, uint16_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToUInt16(const std::string& str, uint16_t& value, uint8_t base = 10);

			bool	StringToInt32(const char* str, int32_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToInt32(const std::string& str, int32_t& value, uint8_t base = 10);

			bool	StringToUInt32(const char* str, uint32_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToUInt32(const std::string& str, uint32_t& value, uint8_t base = 10);

			bool	StringToInt64(const char* str, int64_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToInt64(const std::string& str, int64_t& value, uint8_t base = 10);

			bool	StringToUInt64(const char* str, uint64_t& value, uint8_t base = 10, uint8_t precomputedStrLen = 0);
			bool	StringToUInt64(const std::string& str, uint64_t& value, uint8_t base = 10);

			bool	StringToFloat32(const char* str, float& value, uint8_t precomputedStrLen = 0);
			bool	StringToFloat32(const std::string& str, float& value);

			bool	StringToFloat64(const char* str, double& value, uint8_t precomputedStrLen = 0);
			bool	StringToFloat64(const std::string& str, double& value);
		}
	}
}

#include "StringConversion.inl"
