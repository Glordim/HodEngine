#include "HodEngine/Core/StringConversion.h"
#include "HodEngine/Core/Output.h"

#include <cstdlib>
#include <limits>
#include <locale>
#include <codecvt>

namespace hod
{
	namespace StringConversion
	{
		///@brief Convert String to Int8
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToInt8(const std::string_view& str, int8_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long readedValue = std::strtol(str.data(), &readedEnd, base);
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

		///@brief Convert String to UInt8
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToUInt8(const std::string_view& str, uint8_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long readedValue = std::strtol(str.data(), &readedEnd, base);
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

		///@brief Convert String to Int16
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToInt16(const std::string_view& str, int16_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long readedValue = std::strtol(str.data(), &readedEnd, base);
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

		///@brief Convert String to UInt16
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToUInt16(const std::string_view& str, uint16_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long readedValue = std::strtol(str.data(), &readedEnd, base);
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

		///@brief Convert String to Int32
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToInt32(const std::string_view& str, int32_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long readedValue = std::strtol(str.data(), &readedEnd, base);
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

		///@brief Convert String to UInt32
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToUInt32(const std::string_view& str, uint32_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			unsigned long readedValue = std::strtoul(str.data(), &readedEnd, base);
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

		///@brief Convert String to Int64
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToInt64(const std::string_view& str, int64_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			long long readedValue = std::strtoll(str.data(), &readedEnd, base);
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

		///@brief Convert String to UInt64
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToUInt64(const std::string_view& str, uint64_t& value, uint8_t base)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			unsigned long long readedValue = std::strtoull(str.data(), &readedEnd, base);
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

		///@brief Convert String to Int16
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToFloat32(const std::string_view& str, float& value)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			std::locale::global(std::locale::classic());
			float readedValue = std::strtof(str.data(), &readedEnd);
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

		///@brief Convert String to Float64
		///@param str String to convert
		///@param value Value to out
		///@param base Base to use (default base 10)
		///@return True if conversion is valid, False if out of type limits or if str contain not digit characters
		bool StringToFloat64(const std::string_view& str, double& value)
		{
			const char* strEnd = str.data() + str.size();
			char* readedEnd = nullptr;
			std::locale::global(std::locale::classic());
			double readedValue = std::strtod(str.data(), &readedEnd);
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

		/// @brief 
		/// @param str 
		/// @param result 
		/// @return 
		bool StringToWString(const std::string_view& str, std::wstring& result)
		{
			size_t utf8Size = str.size() + 1;
    		result.resize(utf8Size, L'\0');

    		std::mbstate_t state = std::mbstate_t();
			const std::codecvt<wchar_t, char, std::mbstate_t>& codecvtFacet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());

			const char* strPtr = str.data();
			wchar_t* endResultPtr = &result[utf8Size];
			if (codecvtFacet.in(state, strPtr, strPtr + utf8Size, strPtr, &result[0], &result[0] + utf8Size, endResultPtr) != std::codecvt_base::ok)
			{
				return false;
			}

			return true;
		}
	}
}
