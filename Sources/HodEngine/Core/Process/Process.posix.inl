// Process::Run/Launch of the POSIX platforms (Linux, macOS), included by their Process.<platform>.cpp

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <fmt/format.h>

extern char** environ;

namespace hod::inline core
{
	namespace
	{
		/// @brief What the child execs with, built in the parent: after fork(), the child may only call async-signal-safe functions
		/// (no allocation)
		struct ChildSetup
		{
			String         _executable;
			Vector<String> _argumentStorage;
			Vector<char*>  _arguments;
			Vector<String> _environmentStorage;
			Vector<char*>  _environment;
			String         _workingDirectory;
		};

		std::string_view VariableName(std::string_view entry)
		{
			const size_t separator = entry.find('=');
			return separator == std::string_view::npos ? entry : entry.substr(0, separator);
		}

		void BuildEnvironment(const Process::Desc& desc, ChildSetup& setup)
		{
			for (char** entry = environ; *entry != nullptr; ++entry)
			{
				const std::string_view name = VariableName(*entry);

				bool overridden = false;
				for (const Process::EnvironmentVariable& variable : desc._environment)
				{
					if (name == std::string_view(variable._name))
					{
						overridden = true;
						break;
					}
				}

				if (overridden == false)
				{
					setup._environmentStorage.PushBack(String(*entry));
				}
			}

			for (const Process::EnvironmentVariable& variable : desc._environment)
			{
				if (variable._value.Empty() == false)
				{
					String entry = variable._name;
					entry += "=";
					entry += variable._value;
					setup._environmentStorage.PushBack(std::move(entry));
				}
			}

			// Once all stored: a String may keep its characters inline, moved when the storage grows
			for (String& entry : setup._environmentStorage)
			{
				setup._environment.PushBack(entry.Data());
			}
			setup._environment.PushBack(nullptr);
		}

		/// @brief Looked up in the PATH of the child's environment, as execvp would (not usable: it takes the parent's environment)
		bool ResolveExecutable(const String& program, const ChildSetup& setup, String& executable)
		{
			if (std::string_view(program).find('/') != std::string_view::npos)
			{
				executable = program;
				return true;
			}

			std::string_view path = "/usr/local/bin:/usr/bin:/bin";
			for (const char* entry : setup._environment)
			{
				if (entry != nullptr && VariableName(entry) == "PATH")
				{
					path = std::string_view(entry).substr(5);
					break;
				}
			}

			while (true)
			{
				const size_t           separator = path.find(':');
				const std::string_view directory = path.substr(0, separator);

				String candidate = directory.empty() ? String(".") : String(directory);
				candidate += "/";
				candidate += program;
				if (access(candidate.CStr(), X_OK) == 0)
				{
					executable = std::move(candidate);
					return true;
				}

				if (separator == std::string_view::npos)
				{
					return false;
				}
				path = path.substr(separator + 1);
			}
		}

		bool SetFlags(int fd, int fdFlags, int statusFlags)
		{
			if (fdFlags != 0 && fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | fdFlags) == -1)
			{
				return false;
			}
			if (statusFlags != 0 && fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | statusFlags) == -1)
			{
				return false;
			}
			return true;
		}

		/// @brief Both ends close-on-exec (pipe2 isn't on macOS): only the dup2-ed ones reach the child
		bool MakePipe(int fds[2])
		{
			if (pipe(fds) == -1)
			{
				fds[0] = -1;
				fds[1] = -1;
				return false;
			}
			return SetFlags(fds[0], FD_CLOEXEC, 0) && SetFlags(fds[1], FD_CLOEXEC, 0);
		}

		void CloseIfValid(int& fd)
		{
			if (fd != -1)
			{
				close(fd);
				fd = -1;
			}
		}

		/// @brief fork + execve. Its failure to exec (program not found, working directory missing) comes back through a
		/// close-on-exec pipe: closed without a word, the exec succeeded
		/// @param detach Started by an intermediate child exiting right away, in its own session: nothing to wait for (no zombie)
		/// @return Its pid (the intermediate child's if detached), -1 on failure
		pid_t Start(const Process::Desc& desc, int inputFd, int outputFd, int errorFd, bool detach, String& error)
		{
			ChildSetup setup;
			setup._argumentStorage.PushBack(desc._program);
			for (const String& argument : desc._arguments)
			{
				setup._argumentStorage.PushBack(argument);
			}
			for (String& argument : setup._argumentStorage)
			{
				setup._arguments.PushBack(argument.Data());
			}
			setup._arguments.PushBack(nullptr);

			BuildEnvironment(desc, setup);
			if (desc._workingDirectory.Empty() == false)
			{
				setup._workingDirectory = desc._workingDirectory.GetString();
			}

			if (ResolveExecutable(desc._program, setup, setup._executable) == false)
			{
				error = fmt::format("Can't start {}: not found in the PATH", std::string_view(desc._program)).c_str();
				return -1;
			}

			int execErrorPipe[2];
			if (MakePipe(execErrorPipe) == false)
			{
				error = fmt::format("Can't start {}: pipe failed ({})", std::string_view(desc._program), strerror(errno)).c_str();
				CloseIfValid(execErrorPipe[0]);
				CloseIfValid(execErrorPipe[1]);
				return -1;
			}

			const pid_t pid = fork();
			if (pid == -1)
			{
				error = fmt::format("Can't start {}: fork failed ({})", std::string_view(desc._program), strerror(errno)).c_str();
				CloseIfValid(execErrorPipe[0]);
				CloseIfValid(execErrorPipe[1]);
				return -1;
			}

			if (pid == 0) // Child: async-signal-safe calls only
			{
				if (detach)
				{
					setsid();
					const pid_t grandChild = fork();
					if (grandChild != 0)
					{
						_exit(grandChild == -1 ? 127 : 0);
					}
				}

				// dup2 clears close-on-exec on the new descriptor
				dup2(inputFd, STDIN_FILENO);
				dup2(outputFd, STDOUT_FILENO);
				dup2(errorFd, STDERR_FILENO);

				// The child's own signal mask and dispositions, not the ones of the caller's thread (see Run's SIGPIPE)
				sigset_t emptySet;
				sigemptyset(&emptySet);
				sigprocmask(SIG_SETMASK, &emptySet, nullptr);
				signal(SIGPIPE, SIG_DFL);

				if (setup._workingDirectory.Empty() == false && chdir(setup._workingDirectory.CStr()) == -1)
				{
					const int chdirError = errno;
					(void)write(execErrorPipe[1], &chdirError, sizeof(chdirError));
					_exit(127);
				}

				execve(setup._executable.CStr(), setup._arguments.Data(), setup._environment.Data());
				const int execError = errno;
				(void)write(execErrorPipe[1], &execError, sizeof(execError));
				_exit(127);
			}

			CloseIfValid(execErrorPipe[1]);
			int     execError = 0;
			ssize_t readSize;
			do
			{
				readSize = read(execErrorPipe[0], &execError, sizeof(execError));
			} while (readSize == -1 && errno == EINTR);
			CloseIfValid(execErrorPipe[0]);

			if (detach || readSize == sizeof(execError))
			{
				int status;
				while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
				{
				}
			}

			if (readSize == sizeof(execError))
			{
				error = fmt::format("Can't start {} ({})", std::string_view(desc._program), strerror(execError)).c_str();
				return -1;
			}
			return pid;
		}

		/// @brief Everything the (non-blocking) pipe has now
		/// @return Whether something was read. fd set to -1 once at its end
		bool ReadAvailable(int& fd, String& output)
		{
			bool readSomething = false;
			while (fd != -1)
			{
				char          buffer[65536];
				const ssize_t readSize = read(fd, buffer, sizeof(buffer));
				if (readSize > 0)
				{
					output.Append(std::string_view(buffer, (size_t)readSize));
					readSomething = true;
				}
				else if (readSize == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK))
				{
					CloseIfValid(fd);
				}
				else if (errno != EINTR)
				{
					break; // Nothing more for now
				}
			}
			return readSomething;
		}

		uint64_t MonotonicMilliseconds()
		{
			timespec time;
			clock_gettime(CLOCK_MONOTONIC, &time);
			return (uint64_t)time.tv_sec * 1000 + (uint64_t)time.tv_nsec / 1000000;
		}

		int32_t ExitCode(int status)
		{
			if (WIFEXITED(status))
			{
				return WEXITSTATUS(status);
			}
			if (WIFSIGNALED(status))
			{
				return 128 + WTERMSIG(status); // As the shells report it
			}
			return -1;
		}
	}

	/// @brief
	bool Process::Run(const Desc& desc, Result& result)
	{
		result = Result();

		int  outputPipe[2] = {-1, -1};
		int  errorPipe[2] = {-1, -1};
		int  inputPipe[2] = {-1, -1};
		auto closeAll = [&]()
		{
			for (int* fds : {outputPipe, errorPipe, inputPipe})
			{
				CloseIfValid(fds[0]);
				CloseIfValid(fds[1]);
			}
		};

		bool pipesCreated = MakePipe(outputPipe) && MakePipe(errorPipe);
		if (pipesCreated && desc._input.empty() == false)
		{
			pipesCreated = MakePipe(inputPipe);
		}
		else if (pipesCreated)
		{
			inputPipe[0] = open("/dev/null", O_RDONLY | O_CLOEXEC);
			pipesCreated = (inputPipe[0] != -1);
		}
		if (pipesCreated == false)
		{
			result._error = fmt::format("Failed to create pipes ({})", strerror(errno)).c_str();
			closeAll();
			return false;
		}

		// A write to the input of a child already gone raises SIGPIPE, which would kill the editor: blocked on this thread while
		// it runs (write fails with EPIPE instead), the pending one consumed before unblocking
		sigset_t sigpipeSet;
		sigset_t previousSet;
		sigemptyset(&sigpipeSet);
		sigaddset(&sigpipeSet, SIGPIPE);
		pthread_sigmask(SIG_BLOCK, &sigpipeSet, &previousSet);

		const pid_t pid = Start(desc, inputPipe[0], outputPipe[1], errorPipe[1], false, result._error);

		// Only the child has them now: its exit closes them
		CloseIfValid(inputPipe[0]);
		CloseIfValid(outputPipe[1]);
		CloseIfValid(errorPipe[1]);

		if (pid == -1)
		{
			closeAll();
			pthread_sigmask(SIG_SETMASK, &previousSet, nullptr);
			return false;
		}

		SetFlags(outputPipe[0], 0, O_NONBLOCK);
		SetFlags(errorPipe[0], 0, O_NONBLOCK);
		if (inputPipe[1] != -1)
		{
			SetFlags(inputPipe[1], 0, O_NONBLOCK);
		}

		// Polled rather than read until the end of the pipes: a process it started (an ssh, a credential helper) could keep them
		// open after its exit
		const uint64_t startTime = MonotonicMilliseconds();
		size_t         inputWritten = 0;
		bool           exited = false;
		int            status = 0;
		while (true)
		{
			bool readSomething = ReadAvailable(outputPipe[0], result._output);
			readSomething |= ReadAvailable(errorPipe[0], result._error);

			// Its input written along with its outputs read: it may wait for the whole input before writing, or the opposite
			while (inputPipe[1] != -1)
			{
				if (inputWritten == desc._input.size())
				{
					CloseIfValid(inputPipe[1]); // Its end of input
					break;
				}

				const ssize_t written = write(inputPipe[1], desc._input.data() + inputWritten, desc._input.size() - inputWritten);
				if (written > 0)
				{
					inputWritten += (size_t)written;
				}
				else if (written == -1 && errno == EINTR)
				{
					continue;
				}
				else if (written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
				{
					break; // Full for now
				}
				else
				{
					CloseIfValid(inputPipe[1]); // EPIPE: it stopped reading
				}
			}

			if (exited)
			{
				if (readSomething == false) // What it wrote before its exit is all read
				{
					break;
				}
				continue;
			}

			const pid_t waitResult = waitpid(pid, &status, WNOHANG);
			if (waitResult == pid || (waitResult == -1 && errno != EINTR))
			{
				exited = true;
				continue;
			}

			const bool canceled = (desc._cancel != nullptr && desc._cancel->load() == true);
			const bool timedOut = (desc._timeoutMs != 0 && MonotonicMilliseconds() - startTime >= desc._timeoutMs);
			if (canceled || timedOut)
			{
				result._canceled = canceled;
				result._timedOut = (canceled == false);
				kill(pid, SIGKILL);
				while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
				{
				}
				exited = true;
				continue;
			}

			if (readSomething == false)
			{
				pollfd pollFds[3];
				nfds_t pollFdCount = 0;
				if (outputPipe[0] != -1)
				{
					pollFds[pollFdCount++] = {outputPipe[0], POLLIN, 0};
				}
				if (errorPipe[0] != -1)
				{
					pollFds[pollFdCount++] = {errorPipe[0], POLLIN, 0};
				}
				if (inputPipe[1] != -1)
				{
					pollFds[pollFdCount++] = {inputPipe[1], POLLOUT, 0};
				}
				poll(pollFds, pollFdCount, 5); // Woken up by its outputs, otherwise to check its exit
			}
		}

		result._exitCode = ExitCode(status);
		closeAll();

		sigset_t pendingSet;
		sigpending(&pendingSet);
		if (sigismember(&pendingSet, SIGPIPE) == 1)
		{
			int signalNumber;
			sigwait(&sigpipeSet, &signalNumber);
		}
		pthread_sigmask(SIG_SETMASK, &previousSet, nullptr);
		return true;
	}

	/// @brief
	bool Process::Launch(const Desc& desc)
	{
		int nullFd = open("/dev/null", O_RDWR | O_CLOEXEC);
		if (nullFd == -1)
		{
			OUTPUT_ERROR("Process::Launch: can't open /dev/null ({})", strerror(errno));
			return false;
		}

		String      error;
		const pid_t pid = Start(desc, nullFd, nullFd, nullFd, true, error);
		CloseIfValid(nullFd);
		if (pid == -1)
		{
			OUTPUT_ERROR("Process::Launch: {}", error);
			return false;
		}
		return true;
	}
}
