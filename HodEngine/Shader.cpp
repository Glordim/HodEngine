#include "Shader.h"

#include <vector>
#include <fstream>
#include <iostream>

Shader::Shader()
: shaderId(0)
{
}

Shader::~Shader()
{
    if (this->shaderId != 0)
        glDeleteShader(this->shaderId);
}

bool Shader::load_internal(GLenum shaderType, const std::string& path)
{
    std::ifstream file(path);
    if (file.is_open() == false)
    {
        std::cerr << std::string("Shader : Failed to load Shader \"") + path + "\"" << std::endl;
        return false;
    }

    std::streampos begin = file.tellg();
    file.seekg(0, std::ios::end);
    std::streampos end = file.tellg();
    file.seekg(0, std::ios::beg);
    GLint bufferLen = end - begin;
    char* buffer = new char[bufferLen + 1];
    memset(buffer, '\0', bufferLen + 1);
    file.read(buffer, bufferLen);
    file.close();

    this->shaderId = glCreateShader(shaderType);
    glShaderSource(this->shaderId, 1, &buffer, &bufferLen);
    glCompileShader(this->shaderId);

    delete[] buffer;

    GLint isCompiled = 0;
    glGetShaderiv(this->shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(this->shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(this->shaderId, maxLength, &maxLength, &errorLog[0]);

        std::cerr << std::string("Shader : Failed to compile Shader \"") + path + "\"" << std::endl;

        glDeleteShader(this->shaderId);
        this->shaderId = 0;

        return false;
    }

    return true;
}

GLuint Shader::getShaderId() const
{
    return this->shaderId;
}
