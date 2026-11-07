#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Process/Process.hpp"

#include <win32/file.h>
#include <win32/io.h>
#include <win32/misc.h>
#include <win32/process.h>
#include <win32/threads.h>

#include "HodEngine/Core/StringConversion.hpp"

#include <fmt/format.h>
#include <string>
#include <thread>
#include <wchar.h>

// Missing from WindowsHModular
extern "C"
{
	WCHAR* WINAPI GetEnvironmentStringsW(void);
	BOOL WINAPI   FreeEnvironmentStringsW(WCHAR* environment);
}

#ifndef CREATE_UNICODE_ENVIRONMENT
#define CREATE_UNICODE_ENVIRONMENT 0x00000400
#endif

namespace hod::inline core
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

		STARTUPINFOA si = {};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = hStdOutWrite;
		si.hStdError = hStdErrWrite;
		si.hStdInput = NULL;

		PROCESS_INFORMATION pi = {};

		String commandLine = program.data();
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

	namespace
	{
		/// @brief One argument for CommandLineToArgvW (what the C runtime of the child parses): quoted if needed, the backslashes
		/// before a quote doubled
		void AppendArgument(std::wstring& commandLine, const std::wstring& argument)
		{
			if (commandLine.empty() == false)
			{
				commandLine += L' ';
			}

			if (argument.empty() == false && argument.find_first_of(L" \t\n\v\"") == std::wstring::npos)
			{
				commandLine += argument;
				return;
			}

			commandLine += L'"';
			for (size_t index = 0;; ++index)
			{
				size_t backslashCount = 0;
				while (index < argument.size() && argument[index] == L'\\')
				{
					++index;
					++backslashCount;
				}

				if (index == argument.size())
				{
					commandLine.append(backslashCount * 2, L'\\'); // The closing quote stays one
					break;
				}
				else if (argument[index] == L'"')
				{
					commandLine.append(backslashCount * 2 + 1, L'\\');
					commandLine += L'"';
				}
				else
				{
					commandLine.append(backslashCount, L'\\');
					commandLine += argument[index];
				}
			}
			commandLine += L'"';
		}

		bool BuildCommandLine(const Process::Desc& desc, std::wstring& commandLine)
		{
			std::wstring argument;
			if (StringConversion::StringToWString(desc._program, argument) == false)
			{
				return false;
			}
			AppendArgument(commandLine, argument);

			for (const String& descArgument : desc._arguments)
			{
				argument.clear();
				if (StringConversion::StringToWString(descArgument, argument) == false)
				{
					return false;
				}
				AppendArgument(commandLine, argument);
			}
			return true;
		}

		/// @brief The inherited environment with desc's changes, as CreateProcessW takes it ("NAME=VALUE\0...\0\0", UTF-16)
		bool BuildEnvironmentBlock(const Process::Desc& desc, std::wstring& block)
		{
			struct Variable
			{
				std::wstring _name;
				std::wstring _value;
			};

			Vector<Variable> variables;
			variables.Reserve(desc._environment.Size());
			for (const Process::EnvironmentVariable& environmentVariable : desc._environment)
			{
				Variable variable;
				if (StringConversion::StringToWString(environmentVariable._name, variable._name) == false ||
				    StringConversion::StringToWString(environmentVariable._value, variable._value) == false)
				{
					return false;
				}
				variables.PushBack(std::move(variable));
			}

			WCHAR* inherited = GetEnvironmentStringsW();
			if (inherited == nullptr)
			{
				return false;
			}

			for (const WCHAR* entry = inherited; *entry != L'\0'; entry += wcslen(entry) + 1)
			{
				// The hidden ones ("=C:=C:\...") start with '=': their name goes up to the second one
				const std::wstring_view entryView(entry);
				const size_t            separator = entryView.find(L'=', 1);
				const std::wstring_view name = entryView.substr(0, separator);

				bool overridden = false;
				for (const Variable& variable : variables)
				{
					if (variable._name.size() == name.size() && _wcsnicmp(variable._name.c_str(), name.data(), name.size()) == 0) // Names aren't case sensitive
					{
						overridden = true;
						break;
					}
				}

				if (overridden == false)
				{
					block += entryView;
					block += L'\0';
				}
			}
			FreeEnvironmentStringsW(inherited);

			for (const Variable& variable : variables)
			{
				if (variable._value.empty() == false)
				{
					block += variable._name;
					block += L'=';
					block += variable._value;
					block += L'\0';
				}
			}
			block += L'\0';
			return true;
		}

		/// @brief What the pipe has now, without waiting
		/// @return Whether something was read
		bool ReadAvailable(HANDLE pipe, String& output)
		{
			DWORD available = 0;
			if (PeekNamedPipe(pipe, nullptr, 0, nullptr, &available, nullptr) == FALSE || available == 0)
			{
				return false;
			}

			char  buffer[65536];
			DWORD read = 0;
			if (ReadFile(pipe, buffer, available < sizeof(buffer) ? available : (DWORD)sizeof(buffer), &read, nullptr) == FALSE || read == 0)
			{
				return false;
			}
			output.Append(std::string_view(buffer, read));
			return true;
		}

		void CloseIfValid(HANDLE& handle)
		{
			if (handle != nullptr && handle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle);
			}
			handle = nullptr;
		}

		HANDLE OpenNull(SECURITY_ATTRIBUTES* securityAttributes, DWORD access)
		{
			return CreateFileW(L"NUL", access, FILE_SHARE_READ | FILE_SHARE_WRITE, securityAttributes, OPEN_EXISTING, 0, nullptr);
		}

		/// @brief CreateProcessW with what desc gives
		bool Start(const Process::Desc& desc, STARTUPINFOW& startupInfo, BOOL inheritHandles, DWORD creationFlags, PROCESS_INFORMATION& processInformation, String& error)
		{
			std::wstring commandLine;
			if (BuildCommandLine(desc, commandLine) == false)
			{
				error = "Invalid UTF-8 in the command line";
				return false;
			}

			std::wstring environment;
			if (desc._environment.Empty() == false)
			{
				if (BuildEnvironmentBlock(desc, environment) == false)
				{
					error = "Invalid environment";
					return false;
				}
				creationFlags |= CREATE_UNICODE_ENVIRONMENT;
			}

			std::wstring workingDirectory;
			if (desc._workingDirectory.Empty() == false && StringConversion::StringToWString(desc._workingDirectory.GetString(), workingDirectory) == false)
			{
				error = "Invalid UTF-8 in the working directory";
				return false;
			}

			if (CreateProcessW(nullptr, commandLine.data(), nullptr, nullptr, inheritHandles, creationFlags, environment.empty() ? nullptr : environment.data(),
			                   workingDirectory.empty() ? nullptr : workingDirectory.c_str(), &startupInfo, &processInformation) == FALSE)
			{
				error = fmt::format("Can't start {} (error {})", std::string_view(desc._program), GetLastError()).c_str();
				return false;
			}
			return true;
		}
	}

	/// @brief
	bool Process::Run(const Desc& desc, Result& result)
	{
		result = Result();

		SECURITY_ATTRIBUTES securityAttributes;
		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.bInheritHandle = TRUE;
		securityAttributes.lpSecurityDescriptor = nullptr;

		// The child's ends inherited, not ours
		HANDLE outputRead = nullptr;
		HANDLE outputWrite = nullptr;
		HANDLE errorRead = nullptr;
		HANDLE errorWrite = nullptr;
		HANDLE inputRead = nullptr;
		HANDLE inputWrite = nullptr;
		auto   closeAll = [&]()
		{
			CloseIfValid(outputRead);
			CloseIfValid(outputWrite);
			CloseIfValid(errorRead);
			CloseIfValid(errorWrite);
			CloseIfValid(inputRead);
			CloseIfValid(inputWrite);
		};

		bool pipesCreated = CreatePipe(&outputRead, &outputWrite, &securityAttributes, 0) && CreatePipe(&errorRead, &errorWrite, &securityAttributes, 0);
		if (pipesCreated && desc._input.empty() == false)
		{
			pipesCreated = CreatePipe(&inputRead, &inputWrite, &securityAttributes, 0) && SetHandleInformation(inputWrite, HANDLE_FLAG_INHERIT, 0);
		}
		else if (pipesCreated)
		{
			inputRead = OpenNull(&securityAttributes, GENERIC_READ); // Rather than no stdin at all: some programs fail on it
			pipesCreated = (inputRead != INVALID_HANDLE_VALUE);
		}
		pipesCreated = pipesCreated && SetHandleInformation(outputRead, HANDLE_FLAG_INHERIT, 0) && SetHandleInformation(errorRead, HANDLE_FLAG_INHERIT, 0);
		if (pipesCreated == false)
		{
			result._error = fmt::format("Failed to create pipes (error {})", GetLastError()).c_str();
			closeAll();
			return false;
		}

		STARTUPINFOW startupInfo = {};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESTDHANDLES;
		startupInfo.hStdInput = inputRead;
		startupInfo.hStdOutput = outputWrite;
		startupInfo.hStdError = errorWrite;

		PROCESS_INFORMATION processInformation = {};
		if (Start(desc, startupInfo, TRUE, CREATE_NO_WINDOW, processInformation, result._error) == false)
		{
			closeAll();
			return false;
		}
		CloseHandle(processInformation.hThread);

		// Only the child has them now: its exit closes them
		CloseIfValid(outputWrite);
		CloseIfValid(errorWrite);
		CloseIfValid(inputRead);

		// Written aside: the child may wait for its whole input before writing, or write before reading it
		std::thread inputWriter;
		if (inputWrite != nullptr)
		{
			inputWriter = std::thread(
				[&desc, inputWrite]()
				{
					const char* data = desc._input.data();
					size_t      remaining = desc._input.size();
					while (remaining > 0)
					{
						DWORD written = 0;
						if (WriteFile(inputWrite, data, remaining < 65536 ? (DWORD)remaining : 65536, &written, nullptr) == FALSE) // Broken pipe: it exited
						{
							break;
						}
						data += written;
						remaining -= written;
					}
					CloseHandle(inputWrite);
				});
			inputWrite = nullptr; // The writer's
		}

		// Polled rather than read until the end of the pipes: a process it started (an ssh, a credential helper) could keep them
		// open after its exit
		const ULONGLONG startTime = GetTickCount64();
		bool            exited = false;
		while (true)
		{
			bool readSomething = ReadAvailable(outputRead, result._output);
			readSomething |= ReadAvailable(errorRead, result._error);

			if (exited)
			{
				if (readSomething == false) // What it wrote before its exit is all read
				{
					break;
				}
				continue;
			}

			if (WaitForSingleObject(processInformation.hProcess, readSomething ? 0 : 5) == WAIT_OBJECT_0)
			{
				exited = true;
				continue;
			}

			const bool canceled = (desc._cancel != nullptr && desc._cancel->load() == true);
			const bool timedOut = (desc._timeoutMs != 0 && GetTickCount64() - startTime >= desc._timeoutMs);
			if (canceled || timedOut)
			{
				result._canceled = canceled;
				result._timedOut = (canceled == false);
				TerminateProcess(processInformation.hProcess, 1);
				WaitForSingleObject(processInformation.hProcess, INFINITE);
				exited = true;
			}
		}

		if (inputWriter.joinable())
		{
			inputWriter.join();
		}

		DWORD exitCode = 0;
		GetExitCodeProcess(processInformation.hProcess, &exitCode);
		result._exitCode = (int32_t)exitCode;

		CloseHandle(processInformation.hProcess);
		closeAll();
		return true;
	}

	/// @brief
	bool Process::Launch(const Desc& desc)
	{
		SECURITY_ATTRIBUTES securityAttributes;
		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.bInheritHandle = TRUE;
		securityAttributes.lpSecurityDescriptor = nullptr;

		HANDLE input = OpenNull(&securityAttributes, GENERIC_READ);
		HANDLE output = OpenNull(&securityAttributes, GENERIC_WRITE);

		STARTUPINFOW startupInfo = {};
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESTDHANDLES;
		startupInfo.hStdInput = input;
		startupInfo.hStdOutput = output;
		startupInfo.hStdError = output;

		PROCESS_INFORMATION processInformation = {};
		String              error;
		const bool          started = Start(desc, startupInfo, TRUE, CREATE_NO_WINDOW, processInformation, error);
		CloseIfValid(input);
		CloseIfValid(output);
		if (started == false)
		{
			OUTPUT_ERROR("Process::Launch: {}", error);
			return false;
		}

		CloseHandle(processInformation.hThread);
		CloseHandle(processInformation.hProcess);
		return true;
	}
}
