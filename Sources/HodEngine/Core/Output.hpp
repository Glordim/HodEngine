#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <format>
#include <utility>

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
		Output(Type type, const char* format, Args&&...);

		static const char*	GetTypeName(Type type);
		static const char*	_typeNames[Type::Count];

        std::string _content;
        Type _type;
    };

	/// @brief 
	class HOD_API OutputService
	{
	public:

		static void			AddOutput(const char* fileName, int lineNumber, Output::Type type, const char* format, ...);

	private:

							OutputService() = delete;
							OutputService(const OutputService&) = delete;
							OutputService(OutputService&&) = delete;
							~OutputService() = delete;

		void				operator=(const OutputService&) = delete;
		void				operator=(OutputService&&) = delete;
	};

	/// @brief 
	/// @tparam ...Args 
	/// @param type 
	/// @param format 
	/// @param ...args 
	template<typename... Args>
	Output::Output(Type type, const char* format, Args&&... args)
	: _type(type)
	, _content(std::vformat(format, std::make_format_args(args...)))
	{

	}
}
