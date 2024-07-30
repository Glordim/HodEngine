#pragma once
#include <HodEngine/HodEngine.hpp>
#include <HodEngine/Core/Output/Output.hpp>

#include <string>
#include <format>
#include <vector>

#define OUTPUT_MESSAGE(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Message, __VA_ARGS__);
#define OUTPUT_WARNING(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Warning, __VA_ARGS__);
#define OUTPUT_ERROR(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Error, __VA_ARGS__);

namespace hod
{
	class OutputBucket;

	/// @brief 
	class HOD_API OutputService
	{
	public:

		static void			AddOutput(const char* fileName, int lineNumber, Output::Type type, const char* format);

		template<typename... Args>
		static void			AddOutput(const char* fileName, int lineNumber, Output::Type type, std::format_string<Args...> format, Args&&... args);

		static void			PushBucket(OutputBucket& bucket);
		static void			PopBucket();

	private:

							OutputService() = delete;
							OutputService(const OutputService&) = delete;
							OutputService(OutputService&&) = delete;
							~OutputService() = delete;

		OutputService&		operator=(const OutputService&) = delete;
		OutputService&		operator=(OutputService&&) = delete;

	private:

		static thread_local std::vector<OutputBucket*> _buckets;
	};

	/// @brief 
	/// @tparam ...Args 
	/// @param fileName 
	/// @param lineNumber 
	/// @param type 
	/// @param format 
	/// @param ...args 
	template<typename... Args>
	void OutputService::AddOutput(const char* fileName, int lineNumber, Output::Type type, std::format_string<Args...> format, Args&&... args)
	{
		std::string content = std::vformat(format.get(), std::make_format_args(args...));
		AddOutput(fileName, lineNumber, type, content.c_str());
	}
}
