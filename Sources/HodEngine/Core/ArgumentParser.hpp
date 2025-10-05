#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>

namespace hod
{
	/// @brief
	struct HOD_CORE_API Argument
	{
		char        _shortName = '\0';
		const char* _longName = nullptr;
		const char* _values[32] = {nullptr};
		uint8_t     _valueCount = 0;
	};

	///@brief Utilty class to easly parse short and long option from argc/argv
	class HOD_CORE_API ArgumentParser
	{
	public:
		ArgumentParser(int argc, char** argv);
		ArgumentParser(const ArgumentParser&) = delete;
		ArgumentParser(ArgumentParser&&) = delete;
		~ArgumentParser() = default;

		void operator=(const ArgumentParser&) = delete;
		void operator=(ArgumentParser&&) = delete;

	public:
		const Argument* GetArgument(char shortName, const char* longName) const;

	private:
		Argument _arguments[32];
		uint8_t  _argumentCount = 0;
	};
}
