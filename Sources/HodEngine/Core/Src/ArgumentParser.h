#pragma once

#include <string>
#include <vector>

namespace hod
{
	namespace CORE
	{
		///
		///@brief Utilty class to easly parse short and long option from argc/argv
		///
		class ArgumentParser
		{
		public:

									ArgumentParser() = default;
									ArgumentParser(const ArgumentParser&) = delete;
									ArgumentParser(ArgumentParser&&) = delete;
									~ArgumentParser() = default;

			void					operator = (const ArgumentParser&) = delete;
			void					operator = (ArgumentParser&&) = delete;

		public:

			bool					Register(char shortName, const char* longName, const char* description, bool requireArgument);
			bool					Parse(int argc, char** argv);

			const char*				GetValue(const char* longName);

		private:

			struct Option
			{
				char			_shortName = '\0';
				std::string		_longName;
				bool			_requireArgument;
				const char*		_value = nullptr;
			};

		private:

			std::vector<Option>		_options;
		};
	}
}
