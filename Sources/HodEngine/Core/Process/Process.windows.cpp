#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
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
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		HANDLE hStdOutRead, hStdOutWrite;
		HANDLE hStdErrRead, hStdErrWrite;

		if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0) || !CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0))
		{
			OUTPUT_ERROR("Failed to create pipes ({})", GetLastError());
			return false;
		}

		SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);
		SetHandleInformation(hStdErrRead, HANDLE_FLAG_INHERIT, 0);

		STARTUPINFOA si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = hStdOutWrite;
		si.hStdError = hStdErrWrite;
		si.hStdInput = NULL;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		CHAR resolvedProgram[MAX_PATH];
		ExpandEnvironmentStrings(program.data(), resolvedProgram, MAX_PATH);

		String commandLine = resolvedProgram;
		commandLine += " ";
		commandLine += argument;

		if (!CreateProcessA(NULL, (char*)commandLine.CStr(), NULL, NULL, TRUE, CREATE_NO_WINDOW | (detach ? DETACHED_PROCESS : 0), NULL, NULL, &si, &pi))
		{
			OUTPUT_ERROR("Failed to create process ({})", GetLastError());
			CloseHandle(hStdOutRead);
			CloseHandle(hStdOutWrite);
			CloseHandle(hStdErrRead);
			CloseHandle(hStdErrWrite);
			return false;
		}

		CloseHandle(hStdOutWrite);
		CloseHandle(hStdErrWrite);

		bool result;
		if (detach == true)
		{
			result = true;
		}
		else
		{
			char  buffer[4096];
			DWORD bytesRead;
			BOOL  success;
			DWORD exitCode;

			while (true)
			{
				// Vérifier si le processus est terminé
				if (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
				{
					break;
				}

				// Lire les données de stdout en temps réel
				while (PeekNamedPipe(hStdOutRead, NULL, 0, NULL, &bytesRead, NULL) && bytesRead > 0)
				{
					success = ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
					if (success && bytesRead > 0)
					{
						buffer[bytesRead] = '\0';
						OUTPUT_MESSAGE(buffer);
					}
				}

				// Lire les données de stderr en temps réel
				while (PeekNamedPipe(hStdErrRead, NULL, 0, NULL, &bytesRead, NULL) && bytesRead > 0)
				{
					success = ReadFile(hStdErrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
					if (success && bytesRead > 0)
					{
						buffer[bytesRead] = '\0';
						OUTPUT_ERROR(buffer);
					}
				}

				Sleep(10);
			}

			// Assurez-vous de lire toute sortie restante après la fin du processus
			while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				OUTPUT_MESSAGE(buffer);
			}

			while (ReadFile(hStdErrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				OUTPUT_ERROR(buffer);
			}

			// Récupérer le code de sortie du processus
			GetExitCodeProcess(pi.hProcess, &exitCode);
			result = (exitCode == EXIT_SUCCESS);
		}

		CloseHandle(hStdOutRead);
		CloseHandle(hStdErrRead);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return result;
	}
}
