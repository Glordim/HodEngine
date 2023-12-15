#include "HodEngine/Core/Output.hpp"

#include <cstdio>
#include <cstdarg>

#if defined(_WIN32)
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

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void Output::AddOutput(const char* fileName, int lineNumber, Type type, const char* format, ...)
	{
		va_list va;
		va_start(va, format);

		char buffer[4096];
		vsnprintf(buffer, sizeof(buffer), format, va);

		va_end(va);

		char finalBuffer[4096];
		snprintf(finalBuffer, sizeof(finalBuffer), "%s(%i) : %s : %s\n", fileName, lineNumber, Output::GetTypeName(type), buffer);

		if (type == Type::Message)
		{
			fputs(finalBuffer, stdout);
		}
		else
		{
			fputs(finalBuffer, stderr);
		}

#if defined(_WIN32)
		if (IsDebuggerPresent() == TRUE)
		{
			OutputDebugString(finalBuffer);
		}
#endif
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const char* Output::GetTypeName(Type type)
	{
		return _typeNames[type];
	}
}
