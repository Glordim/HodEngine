#pragma once

#define OUTPUT_MESSAGE(...)		CORE::Output::AddOutput(__FILE__, __LINE__, CORE::Output::Type::Message, __VA_ARGS__);
#define OUTPUT_WARNING(...)		CORE::Output::AddOutput(__FILE__, __LINE__, CORE::Output::Type::Warning, __VA_ARGS__);
#define OUTPUT_ERROR(...)		CORE::Output::AddOutput(__FILE__, __LINE__, CORE::Output::Type::Error, __VA_ARGS__);

namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Output
		{
		public:

			enum Type
			{
				Message = 0,
				Warning,
				Error,
				Assert,
				Count
			};

		public:

			static void			AddOutput(const char* fileName, int lineNumber, Type type, const char* format, ...);

			static const char*	GetTypeName(Type type);

		private:

								Output() = delete;
								Output(const Output&) = delete;
								Output(Output&&) = delete;
								~Output() = delete;

			void				operator=(const Output&) = delete;
			void				operator=(Output&&) = delete;

		private:

			static const char*	_typeNames[Type::Count];
		};
	}
}
