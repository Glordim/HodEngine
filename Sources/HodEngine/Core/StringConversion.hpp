#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <string_view>

namespace hod::inline core
{
	namespace StringConversion
	{
#if defined(PLATFORM_WINDOWS)
		HOD_CORE_API bool   StringToWString(const std::string_view& str, std::wstring& result);
		HOD_CORE_API size_t StringToWStringSize(const std::string_view& str);
		HOD_CORE_API bool   StringToWString(const std::string_view& str, wchar_t* buffer, size_t bufferSize);

		HOD_CORE_API bool   WStringToString(const std::wstring& str, String& result);
		HOD_CORE_API bool   WStringToString(const wchar_t* str, String& result);
		HOD_CORE_API size_t WStringToStringSize(const wchar_t* str, size_t len);
		HOD_CORE_API bool   WStringToString(const wchar_t* str, size_t len, char* buffer, size_t bufferSize);
#endif

		template<typename... Args>
		String StringFormat(const std::string_view& format, Args... args)
		{
			int size = std::snprintf(nullptr, 0, format.data(), args...) + 1; // Extra space for '\0'
			if (size <= 0)
			{
				return String();
			}

			char buffer[4096] = {'\0'};

			String result;
			result.Reserve(size);
			std::snprintf(buffer, size, format.data(), args...);
			buffer[size - 1] = '\0';
			result = buffer;
			return result;
		}
	}
}
