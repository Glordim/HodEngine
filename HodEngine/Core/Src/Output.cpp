#include "Output.h"

#include <cstdio>
#include <cstdarg>

#include <Windows.h>

namespace HOD
{
	namespace CORE
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
				fprintf(stdout, finalBuffer);
			}
			else
			{
				fprintf(stderr, finalBuffer);
			}

			if (IsDebuggerPresent() == TRUE)
			{
				OutputDebugString(finalBuffer);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* Output::GetTypeName(Type type)
		{
			return _typeNames[type];
		}
	}
}
