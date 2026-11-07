#include <gtest/gtest.h>

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

using namespace hod;

// The child of these tests is this executable itself, running ProcessChild.DISABLED_Main: portable, and its arguments are parsed
// by the C runtime (what Process quotes them for). Its answer is between markers, apart from what gtest prints.

namespace
{
	constexpr std::string_view BeginMarker = "@@BEGIN@@\n";
	constexpr std::string_view EndMarker = "@@END@@\n";

	Process::Desc ChildDesc(std::initializer_list<const char*> childArguments)
	{
		Process::Desc desc;
		desc._program = String(::testing::internal::GetArgvs()[0].c_str());
		desc._arguments.PushBack("--gtest_filter=ProcessChild.DISABLED_Main");
		desc._arguments.PushBack("--gtest_also_run_disabled_tests");
		desc._arguments.PushBack("--");
		for (const char* childArgument : childArguments)
		{
			desc._arguments.PushBack(childArgument);
		}
		return desc;
	}

	std::string Between(const String& output)
	{
		std::string text(output.CStr(), output.Size());
		std::erase(text, '\r'); // Text mode outputs of the Windows C runtime
		const size_t begin = text.find(BeginMarker);
		const size_t end = text.rfind(EndMarker);
		if (begin == std::string::npos || end == std::string::npos || end < begin)
		{
			return "<no markers in: " + text + ">";
		}
		return text.substr(begin + BeginMarker.size(), end - begin - BeginMarker.size());
	}
}

TEST(ProcessChild, DISABLED_Main)
{
	std::vector<std::string> arguments;
	bool                     afterSeparator = false;
	for (const std::string& argument : ::testing::internal::GetArgvs())
	{
		if (afterSeparator)
		{
			arguments.push_back(argument);
		}
		else if (argument == "--")
		{
			afterSeparator = true;
		}
	}
	ASSERT_FALSE(arguments.empty());

	const std::string& command = arguments[0];
	int                exitCode = 0;
	std::cout << BeginMarker;
	if (command == "args")
	{
		for (size_t index = 1; index < arguments.size(); ++index)
		{
			std::cout << "[" << arguments[index] << "]\n";
		}
	}
	else if (command == "outputs") // outputs <stdout> <stderr> <exit code>
	{
		std::cout << arguments[1] << "\n";
		std::cerr << BeginMarker << arguments[2] << "\n" << EndMarker;
		exitCode = std::atoi(arguments[3].c_str());
	}
	else if (command == "stdin")
	{
		// Not "cout << cin.rdbuf()": with nothing to copy, it sets cout's failbit, the end marker isn't written
		const std::string input {std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
		std::cout << input;
	}
	else if (command == "env")
	{
		const char* value = std::getenv(arguments[1].c_str());
		std::cout << (value != nullptr ? value : "<unset>");
	}
	else if (command == "cwd")
	{
		std::cout << std::filesystem::current_path().string();
	}
	else if (command == "sleep")
	{
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}
	else if (command == "big") // Both outputs much bigger than a pipe's buffer, interleaved
	{
		const std::string line(1023, 'x');
		for (int index = 0; index < 2048; ++index)
		{
			std::cout << line << '\n';
			std::cerr << line << '\n';
		}
	}
	std::cout << EndMarker;
	std::cout.flush();
	std::cerr.flush();
	std::exit(exitCode);
}

TEST(Process, RunCapturesOutputsAndExitCode)
{
	Process::Result result;
	ASSERT_TRUE(Process::Run(ChildDesc({"outputs", "to out", "to err", "3"}), result));
	EXPECT_EQ(result._exitCode, 3);
	EXPECT_EQ(Between(result._output), "to out\n");
	EXPECT_EQ(Between(result._error), "to err\n");
	EXPECT_FALSE(result._timedOut);
	EXPECT_FALSE(result._canceled);
}

TEST(Process, RunPassesArgumentsAsTheyAre)
{
	Process::Result result;
	// Not a non-ASCII one: the char argv of the Windows C runtime is in the ANSI code page
	ASSERT_TRUE(Process::Run(ChildDesc({"args", "a b", "c\"d", "e\\", "f\\\"g", "", "h\\ i\\\\", "j\tk"}), result));
	EXPECT_EQ(result._exitCode, 0);
	EXPECT_EQ(Between(result._output), "[a b]\n[c\"d]\n[e\\]\n[f\\\"g]\n[]\n[h\\ i\\\\]\n[j\tk]\n");
}

TEST(Process, RunWritesInput)
{
	std::string input;
	for (int index = 0; index < 20000; ++index) // Bigger than a pipe's buffer
	{
		input += "line " + std::to_string(index) + "\n";
	}

	Process::Desc desc = ChildDesc({"stdin"});
	desc._input = input;
	Process::Result result;
	ASSERT_TRUE(Process::Run(desc, result));
	EXPECT_EQ(result._exitCode, 0);
	EXPECT_EQ(Between(result._output), input);
}

TEST(Process, RunWithoutInputGetsNoInput)
{
	Process::Result result;
	ASSERT_TRUE(Process::Run(ChildDesc({"stdin"}), result));
	EXPECT_EQ(Between(result._output), "");
}

TEST(Process, RunSetsEnvironment)
{
	Process::Desc desc = ChildDesc({"env", "HOD_PROCESS_TEST"});
	desc._environment.PushBack({"HOD_PROCESS_TEST", "value with spaces"});
	Process::Result result;
	ASSERT_TRUE(Process::Run(desc, result));
	EXPECT_EQ(Between(result._output), "value with spaces");
}

TEST(Process, RunRemovesEnvironmentVariable)
{
	Process::Desc desc = ChildDesc({"env", "PATH"});
	desc._environment.PushBack({"PATH", ""});
	Process::Result result;
	ASSERT_TRUE(Process::Run(desc, result));
	EXPECT_EQ(Between(result._output), "<unset>");
}

TEST(Process, RunInWorkingDirectory)
{
	const std::filesystem::path directory = std::filesystem::canonical(std::filesystem::temp_directory_path());

	Process::Desc desc = ChildDesc({"cwd"});
	desc._program = String(std::filesystem::absolute(::testing::internal::GetArgvs()[0]).string().c_str()); // Relative to the current one
	desc._workingDirectory = Path(directory.string().c_str());
	Process::Result result;
	ASSERT_TRUE(Process::Run(desc, result));
	EXPECT_EQ(std::filesystem::path(Between(result._output)), directory);
}

TEST(Process, RunReadsBothBigOutputs)
{
	Process::Result result;
	ASSERT_TRUE(Process::Run(ChildDesc({"big"}), result));
	EXPECT_EQ(result._exitCode, 0);
	EXPECT_GT(result._output.Size(), 2048u * 1024u);
	EXPECT_GE(result._error.Size(), 2048u * 1024u);
}

TEST(Process, RunTimesOut)
{
	Process::Desc desc = ChildDesc({"sleep"});
	desc._timeoutMs = 200;
	Process::Result result;

	const auto start = std::chrono::steady_clock::now();
	ASSERT_TRUE(Process::Run(desc, result));
	EXPECT_TRUE(result._timedOut);
	EXPECT_FALSE(result._canceled);
	EXPECT_NE(result._exitCode, 0);
	EXPECT_LT(std::chrono::steady_clock::now() - start, std::chrono::seconds(10));
}

TEST(Process, RunCanBeCanceled)
{
	std::atomic<bool> cancel = false;
	Process::Desc     desc = ChildDesc({"sleep"});
	desc._cancel = &cancel;

	std::thread canceler(
		[&cancel]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			cancel = true;
		});

	Process::Result result;
	const auto      start = std::chrono::steady_clock::now();
	ASSERT_TRUE(Process::Run(desc, result));
	canceler.join();
	EXPECT_TRUE(result._canceled);
	EXPECT_FALSE(result._timedOut);
	EXPECT_LT(std::chrono::steady_clock::now() - start, std::chrono::seconds(10));
}

TEST(Process, RunFailsOnMissingProgram)
{
	Process::Desc desc;
	desc._program = "hod-this-program-does-not-exist";
	Process::Result result;
	EXPECT_FALSE(Process::Run(desc, result));
	EXPECT_FALSE(result._error.Empty());
}

TEST(Process, LaunchDoesNotWait)
{
	const auto    start = std::chrono::steady_clock::now();
	Process::Desc desc = ChildDesc({"outputs", "a", "b", "0"});
	EXPECT_TRUE(Process::Launch(desc));
	EXPECT_LT(std::chrono::steady_clock::now() - start, std::chrono::seconds(5));

	Process::Desc missing;
	missing._program = "hod-this-program-does-not-exist";
	EXPECT_FALSE(Process::Launch(missing));
}
