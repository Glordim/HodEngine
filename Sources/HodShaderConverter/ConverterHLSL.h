#pragma once

#include "Converter.h"

namespace hod
{
    class ConverterHLSL : public Converter
    {
    public:
		ConverterHLSL() = default;
		ConverterHLSL(const ConverterHLSL&) = delete;
		ConverterHLSL(ConverterHLSL&&) = delete;
		~ConverterHLSL() = default;

		ConverterHLSL&    operator=(const ConverterHLSL&) = delete;
		ConverterHLSL&    operator=(ConverterHLSL&&) = delete;

    public:

        bool Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens) override;
    };
}
