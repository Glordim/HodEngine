#include "HodEngine/Core/Process/Process.hpp"

#include <Windows.h>

namespace hod
{
	/// @brief 
	/// @param program 
	/// @param argument 
	/// @return 
	bool Process::Create(const std::string_view& program, const std::string_view& argument, bool detach)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		CHAR resolvedProgram[MAX_PATH];
		ExpandEnvironmentStrings(program.data(), resolvedProgram, MAX_PATH);

		std::string commandLine = resolvedProgram;
		commandLine += " ";
		commandLine += argument;

		if (!CreateProcessA(NULL, (char*)commandLine.c_str(), NULL, NULL, FALSE, detach ? DETACHED_PROCESS : 0, NULL, NULL, &si, &pi)) 
		{
			//std::cerr << std::format("CreateProcess failed ({})\n", GetLastError());
			return false;
		}

		if (detach == false)
		{
			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE );
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return true;
	}
}
