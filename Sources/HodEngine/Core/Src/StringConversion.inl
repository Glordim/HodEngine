#pragma once

#include <string>
#include <vector>

namespace hod
{
	namespace CORE
	{
		namespace StringConversion
		{
			///
			///@brief Convert String to Int8
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToInt8(const std::string& str, int8_t& value, uint8_t base)
			{
				return StringToInt8(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to UInt8
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToUInt8(const std::string& str, uint8_t& value, uint8_t base)
			{
				return StringToUInt8(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to Int16
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToInt16(const std::string& str, int16_t& value, uint8_t base)
			{
				return StringToInt16(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to UInt16
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToUInt16(const std::string& str, uint16_t& value, uint8_t base)
			{
				return StringToUInt16(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to Int32
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToInt32(const std::string& str, int32_t& value, uint8_t base)
			{
				return StringToInt32(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to UInt32
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToUInt32(const std::string& str, uint32_t& value, uint8_t base)
			{
				return StringToUInt32(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to Int64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToInt64(const std::string& str, int64_t& value, uint8_t base)
			{
				return StringToInt64(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to UInt64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToUInt64(const std::string& str, uint64_t& value, uint8_t base)
			{
				return StringToUInt64(str.c_str(), value, base, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to Float32
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToFloat32(const std::string& str, float& value)
			{
				return StringToFloat32(str.c_str(), value, static_cast<uint8_t>(str.size()));
			}

			///
			///@brief Convert String to Float64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			inline bool StringToFloat64(const std::string& str, double& value)
			{
				return StringToFloat64(str.c_str(), value, static_cast<uint8_t>(str.size()));
			}
		}
	}
}
