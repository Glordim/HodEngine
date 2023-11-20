#pragma once

#include <vector>
#include <string>

#include "Lexer.h"

namespace hod
{
    class Converter
    {
    public:
		Converter() = default;
		Converter(const Converter&) = delete;
		Converter(Converter&&) = delete;
		~Converter() = default;

		Converter&    operator=(const Converter&) = delete;
		Converter&    operator=(Converter&&) = delete;

	public:
	
		virtual bool Convert(const std::vector<ShaderLangToken>& tokens) = 0;

		const std::string& GetResult() const { return _result; }

    protected:

        std::string _result;
    };
}
