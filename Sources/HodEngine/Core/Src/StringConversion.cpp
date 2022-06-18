#include "StringConversion.h"
#include "Output.h"

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
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToInt8(const char* str, int8_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long readedValue = std::strtol(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<int8_t>::min() || readedValue > std::numeric_limits<int8_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToInt8: unable to convert %s to Int8", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<int8_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to UInt8
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToUInt8(const char* str, uint8_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long readedValue = std::strtol(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<uint8_t>::min() || readedValue > std::numeric_limits<uint8_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToUInt8: unable to convert %s to UInt8", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<uint8_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to Int16
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToInt16(const char* str, int16_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long readedValue = std::strtol(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<int16_t>::min() || readedValue > std::numeric_limits<int16_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToInt16: unable to convert %s to Int16", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<int16_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to UInt16
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToUInt16(const char* str, uint16_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long readedValue = std::strtol(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<uint16_t>::min() || readedValue > std::numeric_limits<uint16_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToUInt16: unable to convert %s to UInt16", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<uint16_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to Int32
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToInt32(const char* str, int32_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long readedValue = std::strtol(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<int32_t>::min() || readedValue > std::numeric_limits<int32_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToInt32: unable to convert %s to Int32 (invalid format or out of limits)", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<int32_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to UInt32
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToUInt32(const char* str, uint32_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				unsigned long readedValue = std::strtoul(str, &readedEnd, base);
				if (readedEnd != strEnd || readedValue < std::numeric_limits<uint32_t>::min() || readedValue > std::numeric_limits<uint32_t>::max())
				{
					OUTPUT_ERROR("StringConversion::StringToUInt32: unable to convert %s to UInt32", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<uint32_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to Int64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToInt64(const char* str, int64_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				long long readedValue = std::strtoll(str, &readedEnd, base);
				if (readedEnd != strEnd)
				{
					OUTPUT_ERROR("StringConversion::StringToInt64: unable to convert %s to Int64", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<int64_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to UInt64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToUInt64(const char* str, uint64_t& value, uint8_t base, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				unsigned long long readedValue = std::strtoull(str, &readedEnd, base);
				if (readedEnd != strEnd)
				{
					OUTPUT_ERROR("StringConversion::StringToUInt64: unable to convert %s to UInt64", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<uint64_t>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to Int16
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToFloat32(const char* str, float& value, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				float readedValue = std::strtof(str, &readedEnd);
				if (readedEnd != strEnd)
				{
					OUTPUT_ERROR("StringConversion::StringToFloat32: unable to convert %s to Float32", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<float>(readedValue);
					return true;
				}
			}

			///
			///@brief Convert String to Float64
			///
			///@param str String to convert
			///@param value Value to out
			///@param base Base to use (default base 10)
			///@param precomputedStrLen Length of the String param, 0 => recompute
			///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
			///
			bool StringToFloat64(const char* str, double& value, uint8_t precomputedStrLen)
			{
				if (precomputedStrLen == 0)
				{
					precomputedStrLen = static_cast<uint8_t>(std::strlen(str));
				}

				const char* strEnd = str + precomputedStrLen;
				char* readedEnd = nullptr;
				double readedValue = std::strtod(str, &readedEnd);
				if (readedEnd != strEnd)
				{
					OUTPUT_ERROR("StringConversion::StringToFloat64: unable to convert %s to Float64", str);
					value = 0;
					return false;
				}
				else
				{
					value = static_cast<double>(readedValue);
					return true;
				}
			}
		}
	}
}
