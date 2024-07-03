#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string_view>
#include <string>

namespace hod
{
	namespace StringConversion
	{
		#if defined(PLATFORM_WINDOWS)
		HOD_API bool	StringToWString(const std::string_view& str, std::wstring& result);
		HOD_API bool	WStringToString(const std::wstring& str, std::string& result);
		#endif

		template<typename ... Args>
		HOD_API std::string StringFormat(const std::string_view& format, Args ... args )
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
