#include "HodEngine/Core/Output.hpp"

#include <cstdio>
#include <cstdarg>

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

namespace hod
{
	const char* Output::_typeNames[Type::Count] = {
		"Message",
		"Warning",
		"Error",
		"Assert"
	};

	/// @brief 
	/// @param type 
	/// @param content 
	Output::Output(Type type, const std::string_view& content)
	: _type(type)
	, _content(content)
	{

	}

	/// @brief 
	/// @param fileName 
	/// @param lineNumber 
	/// @param type 
	/// @param format 
	/// @param  
	void OutputService::AddOutput(const char* fileName, int lineNumber, Output::Type type, const char* format, ...)
	{
		va_list va;
		va_start(va, format);

		char buffer[4096];
		vsnprintf(buffer, sizeof(buffer), format, va);

		va_end(va);

		char finalBuffer[4096];
		snprintf(finalBuffer, sizeof(finalBuffer), "%s(%i) : %s : %s\n", fileName, lineNumber, Output::GetTypeName(type), buffer);

		if (type == Output::Type::Message)
		{
			fputs(finalBuffer, stdout);
		}
		else
		{
			fputs(finalBuffer, stderr);
		}

#if defined(PLATFORM_WINDOWS)
		if (IsDebuggerPresent() == TRUE)
		{
			OutputDebugString(finalBuffer);
		}
#endif
	}

	/// @brief 
	/// @param type 
	/// @return 
	const char* Output::GetTypeName(Type type)
	{
		return _typeNames[type];
	}
}
