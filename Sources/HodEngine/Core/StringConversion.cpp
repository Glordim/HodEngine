#include "HodEngine/Core/StringConversion.hpp"
#include "HodEngine/Core/Output.hpp"

#include <cstdlib>
#include <limits>
#include <locale>
#include <codecvt>

namespace hod
{
	namespace StringConversion
	{
		/// @brief 
		/// @param str 
		/// @param result 
		/// @return 
		bool StringToWString(const std::string_view& str, std::wstring& result)
		{
			size_t utf8Size = str.size() + 1;
    		result.resize(str.size(), L'\0');

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

		/// @brief 
		/// @param str 
		/// @param result 
		/// @return 
		bool WStringToString(const std::wstring& str, std::string& result)
		{
			size_t utf8Size = str.size() + 1;
    		result.resize(str.size(), L'\0');

    		std::mbstate_t state = std::mbstate_t();
			const std::codecvt<char, wchar_t, std::mbstate_t>& codecvtFacet = std::use_facet<std::codecvt<char, wchar_t, std::mbstate_t>>(std::locale());

			const wchar_t* strPtr = str.data();
			char* endResultPtr = &result[utf8Size];
			if (codecvtFacet.in(state, strPtr, strPtr + utf8Size, strPtr, &result[0], &result[0] + utf8Size, endResultPtr) != std::codecvt_base::ok)
			{
				return false;
			}

			return true;
		}
	}
}
