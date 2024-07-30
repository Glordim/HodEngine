#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Process/Process.hpp"

#include <sys/wait.h>
#include <unistd.h>
#include <string.h> // strerror

#include <vector>
#include <string>

namespace hod
{
	/// @brief 
	/// @param program 
	/// @param argument 
	/// @return 
	bool Process::Create(const std::string_view& program, const std::string_view& argument, bool detach)
	{
		bool result = false;

		pid_t pid = fork(); // Crée un nouveau processus
		if (pid == -1)
		{
			//std::cerr << std::format("CreateProcess failed ({})\n", strerror(errno));
			return false;
		}

		if (pid == 0) // Child
		{
			std::string commandLine(program);
			commandLine += " ";
			commandLine += argument;

			std::vector<const char*> argv;
			char* begin = commandLine.data();
			char* cursor = begin;
			while (*cursor != '\0')
			{
				if (*cursor == ' ')
				{
					*cursor = '\0';					
					argv.push_back(begin);

					begin = cursor + 1;
					cursor = begin;
				}
				else
				{
					++cursor;
				}
			}
			argv.push_back(begin);

			execvp(argv[0], (char *const *)argv.data());
			//std::cerr << "Exec failed" << std::endl;
			return false;
		}
		else // Parent
		{
			if (detach == true)
			{
				result = true;
			}
			else
			{
				int status;
				waitpid(pid, &status, 0);

				if (WIFEXITED(status))
				{
					//std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
					result = true;
				}
				else
				{
					//std::cerr << "Child did not exit normally" << std::endl;
					result = false;
				}
			}
		}

		return result;
	}
}
