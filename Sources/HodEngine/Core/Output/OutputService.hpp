#pragma once
#include "HodEngine/Core/Export.hpp"
#include <HodEngine/Core/Output/Output.hpp>

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <fmt/format.h>

#define OUTPUT_MESSAGE(...) hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Message, __VA_ARGS__);
#define OUTPUT_WARNING(...) hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Warning, __VA_ARGS__);
#define OUTPUT_ERROR(...) hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Error, __VA_ARGS__);

namespace hod
{
	class OutputBucket;

	/// @brief
	class HOD_CORE_API OutputService
	{
	public:
		static void AddOutput(const char* fileName, int lineNumber, Output::Type type, const char* format);

		template<typename... Args>
		static void AddOutput(const char* fileName, int lineNumber, Output::Type type, fmt::format_string<Args...> format, Args&&... args);

		static void PushBucket(OutputBucket& bucket);
		static void PopBucket();

	private:
		OutputService() = delete;
		OutputService(const OutputService&) = delete;
		OutputService(OutputService&&) = delete;
		~OutputService() = delete;

		OutputService& operator=(const OutputService&) = delete;
		OutputService& operator=(OutputService&&) = delete;

	private:
		static void PlatformOutput(Output::Type type, const String& content);
	};

	/// @brief
	/// @tparam ...Args
	/// @param fileName
	/// @param lineNumber
	/// @param type
	/// @param format
	/// @param ...args
	template<typename... Args>
	void OutputService::AddOutput(const char* fileName, int lineNumber, Output::Type type, fmt::format_string<Args...> format, Args&&... args)
	{
		String content = fmt::vformat(format.get(), fmt::make_format_args(args...)).c_str();
		AddOutput(fileName, lineNumber, type, content.CStr());
	}
}
