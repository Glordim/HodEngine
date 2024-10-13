#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/ArgumentParser.hpp"

#include <cstring>

namespace hod
{
	/// @brief 
	/// @param argc 
	/// @param argv 
	ArgumentParser::ArgumentParser(int argc, char** argv)
	{
		Argument* currentArgument = nullptr;
		for (uint8_t index = 1; index < argc; ++index) // start at 1, skip executable name
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
				if (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0')
				{
					currentArgument = _arguments + _argumentCount;
					++_argumentCount;

					currentArgument->_shortName = arg[1];
				}
				else if (arg[0] == '-' && arg[1] == '-' && arg[2] != '\0')
				{
					currentArgument = _arguments + _argumentCount;
					++_argumentCount;

					currentArgument->_longName = arg + 2;
				}
			}
			else if (currentArgument != nullptr)
			{
				currentArgument->_values[currentArgument->_valueCount] = arg;
				++currentArgument->_valueCount;
			}
		}
	}

	/// @brief 
	/// @param shortName 
	/// @param longName 
	/// @return 
	const Argument* ArgumentParser::GetArgument(char shortName, const char* longName) const
	{
		for (uint8_t index = 0; index < _argumentCount; ++index)
		{
			const Argument* argument = _arguments + index;
			if (argument->_shortName == shortName || std::strcmp(argument->_longName, longName) == 0)
			{
				return argument;
			}
		}
		return nullptr;
	}
}
