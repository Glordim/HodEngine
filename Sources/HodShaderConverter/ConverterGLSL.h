#pragma once

#include "Converter.h"

namespace hod
{
    class ConverterGLSL : public Converter
    {
    public:
		ConverterGLSL() = default;
		ConverterGLSL(const ConverterGLSL&) = delete;
		ConverterGLSL(ConverterGLSL&&) = delete;
		~ConverterGLSL() = default;

		ConverterGLSL&    operator=(const ConverterGLSL&) = delete;
		ConverterGLSL&    operator=(ConverterGLSL&&) = delete;

    public:

        bool Convert(const std::vector<ShaderLangToken>& tokens) override;
    };
}
