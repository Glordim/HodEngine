#include "ArgumentParser.h"
#include "Output.h"

namespace HOD
{
	namespace CORE
	{
		///
		///@brief Register an option, must be call before Parse() function
		///
		///@param shortName Single character option name, exemple 'd', use '\\0' to ignore
		///@param longName Multi character option name, exemple "delete", must be valid (len > 0)
		///@param description Description printed with help command, can be nullptr
		///@return True if the registration successful, False if invalid argument or if another option already have the same short or long name
		///
		bool ArgumentParser::Register(char shortName, const char* longName, const char* description)
		{
			if (longName == nullptr || std::strlen(longName) == 0)
			{
				OUTPUT_ERROR("ArgumentParser: unable to register an option without long name");
				return false;
			}

			for (const Option& option : _options)
			{
				if (option._shortName != '\0' && option._shortName == shortName)
				{
					OUTPUT_ERROR("ArgumentParser: an option with the same short name has been already registered : '%c'", shortName);
					return false;
				}
				if (option._longName == longName)
				{
					OUTPUT_ERROR("ArgumentParser: an option with the same long name has been already registered : \"%s\"", longName);
					return false;
				}
			}

			_options.resize(_options.size() + 1);
			Option& newOption = _options[_options.size() - 1];
			newOption._shortName = shortName;
			newOption._longName = longName;
			return true;
		}

		///
		///@brief Parse option from argc and argv based on registred options by the Register() function
		///
		///@param argc Pass argc param received by the main() function
		///@param argv Pass argv param received by the main() function
		///@return True if the parsing successful, False if an unregistred option is founded
		///
		bool ArgumentParser::Parse(int argc, char** argv)
		{
			for (uint8_t index = 1; index < argc; ++index)
			{
				char* arg = argv[index];
				char* end = arg;
				while (*end != ' ' && *end != '\0')
				{
					++end;
				}
				*end = '\0';

				if (arg[0] == '-')
				{
					if (arg[0] == '-' && arg[1] == '-' && arg[2] != '\0')
					{
						bool founded = false;
						for (Option& option : _options)
						{
							if (option._longName == (arg + 2))
							{
								if (option._value != nullptr)
								{
									OUTPUT_ERROR("ArgumentParser: option used twice : \"%s\"", arg + 2);
									return false;
								}
								if (index + 1 < argc)
								{
									arg = argv[index + 1];
									if (arg[0] != '-')
									{
										option._value = arg;
										++index;
									}
								}
								else
								{
									
								}
								founded = true;
								break;
							}
						}
						if (founded == false)
						{
							OUTPUT_ERROR("ArgumentParser: unknonw option : \"%s\"", arg + 2);
							return false;
						}
					}
					else if (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0')
					{
						bool founded = false;
						for (Option& option : _options)
						{
							if (option._shortName == arg[1])
							{
								if (option._value != nullptr)
								{
									OUTPUT_ERROR("ArgumentParser: option used twice : '%c'", arg[1]);
									return false;
								}
								if (index + 1 < argc)
								{
									arg = argv[index + 1];
									if (arg[0] != '-')
									{
										option._value = arg;
										++index;
									}
								}
								founded = true;
								break;
							}
						}
						if (founded == false)
						{
							OUTPUT_ERROR("ArgumentParser: unknonw option : '%c'", arg[1]);
							return false;
						}
					}
					else
					{
						OUTPUT_ERROR("ArgumentParser: invalid argument parsing : \"%s\"", arg);
						return false;
					}
				}
			}
			return true;
		}

		///
		///@brief Get the value associated to an option
		///
		///@param longName Long name used to register option
		///@return const char* Value passed for this option, nullptr if no specified in argc/argv
		///
		const char* ArgumentParser::GetValue(const char* longName)
		{
			for (const Option& option : _options)
			{
				if (option._longName == longName)
				{
					return option._value;
				}
			}
			return nullptr;
		}
	}
}
