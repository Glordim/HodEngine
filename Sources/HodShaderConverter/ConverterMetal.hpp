#pragma once

#include "Converter.hpp"

namespace hod
{
    class ConverterMetal : public Converter
    {
    public:
		ConverterMetal() = default;
		ConverterMetal(const ConverterMetal&) = delete;
		ConverterMetal(ConverterMetal&&) = delete;
		~ConverterMetal() = default;

		ConverterMetal&    operator=(const ConverterMetal&) = delete;
		ConverterMetal&    operator=(ConverterMetal&&) = delete;

    public:

        bool Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens) override;
    };
}
