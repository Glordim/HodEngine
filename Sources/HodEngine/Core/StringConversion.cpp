#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/StringConversion.hpp"

#include <cstdlib>

#if defined(PLATFORM_WINDOWS)
	#include <win32/misc.h> // WideCharToMultiByte
#endif

namespace hod::inline core
{
	namespace StringConversion
	{
#if defined(PLATFORM_WINDOWS)
		/// @brief
		/// @param str
		/// @param result
		/// @return
		bool StringToWString(const std::string_view& str, std::wstring& result)
		{
			size_t sizeNeeded = StringToWStringSize(str);
			if (sizeNeeded == 0)
				return str.empty();
			result.resize(sizeNeeded, L'\0');
			return StringToWString(str, &result[0], sizeNeeded);
		}

		/// @brief
		/// @param str
		/// @param result
		/// @return
		bool WStringToString(const std::wstring& str, String& result)
		{
			if (str.empty())
				return true;
			size_t sizeNeeded = WStringToStringSize(str.c_str(), str.size());
			if (sizeNeeded == 0)
				return false;
			result.Resize(sizeNeeded);
			return WStringToString(str.c_str(), str.size(), result.Data(), sizeNeeded);
		}

		bool WStringToString(const wchar_t* str, String& result)
		{
			if (str == nullptr || str[0] == L'\0')
				return str != nullptr;
			size_t len = std::char_traits<wchar_t>::length(str);
			size_t sizeNeeded = WStringToStringSize(str, len);
			if (sizeNeeded == 0)
				return false;
			result.Resize(sizeNeeded);
			return WStringToString(str, len, result.Data(), sizeNeeded);
		}

		size_t StringToWStringSize(const std::string_view& str)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
			return size > 0 ? static_cast<size_t>(size) : 0;
		}

		bool StringToWString(const std::string_view& str, wchar_t* buffer, size_t bufferSize)
		{
			int written = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), buffer, static_cast<int>(bufferSize));
			return written > 0;
		}

		size_t WStringToStringSize(const wchar_t* str, size_t len)
		{
			int size = WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
			return size > 0 ? static_cast<size_t>(size) : 0;
		}

		bool WStringToString(const wchar_t* str, size_t len, char* buffer, size_t bufferSize)
		{
			int written = WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), buffer, static_cast<int>(bufferSize), nullptr, nullptr);
			return written > 0;
		}
#endif
	}
}
