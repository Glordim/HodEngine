#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <string_view>

namespace hod
{
	namespace StringConversion
	{
#if defined(PLATFORM_WINDOWS)
		HOD_CORE_API bool StringToWString(const std::string_view& str, std::wstring& result);
		HOD_CORE_API bool WStringToString(const std::wstring& str, String& result);
#endif

		template<typename... Args>
		HOD_CORE_API String StringFormat(const std::string_view& format, Args... args)
		{
			int size = std::snprintf(nullptr, 0, format.data(), args...) + 1; // Extra space for '\0'
			if (size <= 0)
			{
				return String();
			}

			char buffer[4096] = {'\0'};

			String result;
			result.reserve(size);
			std::snprintf(buffer, size, format.data(), args...);
			buffer[size - 1] = '\0';
			result = buffer;
			return result;
		}
	}
}
