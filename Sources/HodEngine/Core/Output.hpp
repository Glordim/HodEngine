#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <format>
#include <utility>
#include <mutex>
#include <vector>

#define OUTPUT_MESSAGE(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Message, __VA_ARGS__);
#define OUTPUT_WARNING(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Warning, __VA_ARGS__);
#define OUTPUT_ERROR(...)		hod::OutputService::AddOutput(__FILE__, __LINE__, hod::Output::Type::Error, __VA_ARGS__);

namespace hod
{
	/// @brief 
	struct HOD_API Output
    {
		enum Type
		{
			Message = 0,
			Warning,
			Error,
			Assert,
			Count
		};

		Output(Type type, const std::string_view& content);

		template<typename... Args>
		Output(Type type, std::format_string<Args...> format, Args&&...);

		static const char*	GetTypeName(Type type);
		static const char*	_typeNames[Type::Count];

        std::string _content;
        Type _type;
    };

	/// @brief 
	class HOD_API OutputBucket
	{
	public:

		void Clear();

		std::mutex& GetLock() const;
		const std::vector<Output>& GetOutputs() const;
		void AddOutput(const Output& output);

	private:

		std::vector<Output> _outputs;
		mutable std::mutex	_lock;
	};

	/// @brief 
	class HOD_API ScopedEnableOutputBucket
	{
	public:
		ScopedEnableOutputBucket(OutputBucket& outputBucket);
		~ScopedEnableOutputBucket();
	private:
		OutputBucket& _outputBucket;
	};

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

		void				operator=(const OutputService&) = delete;
		void				operator=(OutputService&&) = delete;

	private:

		static thread_local std::vector<OutputBucket*> _buckets;
	};

	/// @brief 
	/// @tparam ...Args 
	/// @param type 
	/// @param format 
	/// @param ...args 
	template<typename... Args>
	Output::Output(Type type, std::format_string<Args...> format, Args&&... args)
	: _type(type)
	, _content(std::vformat(format.get(), std::make_format_args(args...)))
	{

	}

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
