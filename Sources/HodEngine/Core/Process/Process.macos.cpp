#include "HodEngine/Core/Process/Process.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

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

		int stdout_pipe[2];
		int stderr_pipe[2];

		if (pipe(stdout_pipe) == -1)
		{
			OUTPUT_ERROR("stdout pipe failed: {}", strerror(errno));
			return 1;
		}

		if (pipe(stderr_pipe) == -1)
		{
			OUTPUT_ERROR("stderr pipe failed: {}", strerror(errno));
			return 1;
		}

		pid_t pid = fork(); // Crée un nouveau processus
		if (pid == -1)
		{
			OUTPUT_ERROR("CreateProcess failed ({})", strerror(errno));
			return false;
		}

		if (pid == 0) // Child
		{
			close(stdout_pipe[0]); // Close the read end of stdout pipe
        	dup2(stdout_pipe[1], STDOUT_FILENO); // Redirect stdout to the pipe
        	close(stdout_pipe[1]); // Close the write end of stdout pipe (it is duplicated now)

        	close(stderr_pipe[0]); // Close the read end of stderr pipe
        	dup2(stderr_pipe[1], STDERR_FILENO); // Redirect stderr to the pipe
        	close(stderr_pipe[1]); // Close the write end of stderr pipe (it is duplicated now)

			std::string commandLine(program);
			commandLine += " ";
			commandLine += argument;
			//OUTPUT_ERROR(commandLine.c_str());

			std::vector<const char*> argv;
			char* begin = commandLine.data();
			char* cursor = begin;
			bool escaped = 0;
			while (*cursor != '\0')
			{
				if (*cursor == ' ' && escaped == false)
				{
					*cursor = '\0';					
					argv.push_back(begin);

					begin = cursor + 1;
					cursor = begin;
				}
				else
				{
					if (*cursor == '"')
					{
						if (escaped == false)
						{
							escaped = true;
							begin = cursor + 1;
						}
						else
						{
							escaped = false;
							*cursor = '\0';
						}
					}
					++cursor;
				}
			}
			argv.push_back(begin);
			argv.push_back(nullptr);

			if (execvp(argv[0], (char *const *)argv.data()) == -1)
			{
				//OUTPUT_ERROR("Error executing program: {}", strerror(errno));
				exit(EXIT_FAILURE);
       		}
			exit(EXIT_SUCCESS);
		}
		else // Parent
		{
			if (detach == true)
			{
				result = true;
			}
			else
			{
				// This is the parent process
				close(stdout_pipe[1]); // Close the write end of stdout pipe
				close(stderr_pipe[1]); // Close the write end of stderr pipe

				fd_set stdout_pipe_fds;
				fd_set stderr_pipe_fds;

				char buffer[1024];

				int status;
				
				bool finished = false;
				while (finished != true)
				{
					int waitResult = waitpid(pid, &status, WNOHANG);
					if (waitResult == -1)
					{
						perror("waitpid");
						return false;
					}
					else
					{
						finished = (waitResult != 0);
					}
					
					FD_ZERO(&stdout_pipe_fds);
					FD_SET(stdout_pipe[0], &stdout_pipe_fds);

					struct timeval timeout;
					timeout.tv_sec = 0.0f;
					timeout.tv_usec = 0.0f;
					int ret = select(stdout_pipe[0] + 1, &stdout_pipe_fds, NULL, NULL, &timeout);
					if (ret == -1)
					{
						perror("select");
						return false;
					}

					if (FD_ISSET(stdout_pipe[0], &stdout_pipe_fds))
					{
						size_t n = read(stdout_pipe[0], buffer, sizeof(buffer));
						if (n == 0)
						{
							//printf("Child process terminated\n");
							//finished = true;
						}
						else if (n > 0)
						{
							buffer[n] = '\0';
							OUTPUT_MESSAGE(buffer);
						}
					}

					FD_ZERO(&stderr_pipe_fds);
					FD_SET(stderr_pipe[0], &stderr_pipe_fds);

					ret = select(stderr_pipe[0] + 1, &stderr_pipe_fds, NULL, NULL, &timeout);
					if (ret == -1)
					{
						perror("select");
						return false;
					}

					if (FD_ISSET(stderr_pipe[0], &stderr_pipe_fds))
					{
						size_t n = read(stderr_pipe[0], buffer, sizeof(buffer));
						if (n == 0)
						{
							//printf("Child process terminated\n");
							//finished = true;
						}
						else if (n > 0)
						{
							buffer[n] = '\0';
							OUTPUT_ERROR(buffer);
						}
					}
				}
				
				close(stdout_pipe[0]); // Close the read end of stdout pipe
        		close(stderr_pipe[0]); // Close the read end of stderr pipe

				if (WIFEXITED(status))
				{
					//std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
					result = (WEXITSTATUS(status) == EXIT_SUCCESS);
				}
				else if (WIFSIGNALED(status))
				{
            		//printf("Child killed by signal %d\n", WTERMSIG(status));
					result = false;
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
