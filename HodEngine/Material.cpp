#include "Material.h"

#include "VertexShader.h"
#include "FragmentShader.h"

#include "Texture.h"

#include <vector>

#include <iostream>

Material::Material()
    : programId(0)
{

}

Material::~Material()
{
    if (this->programId != 0)
        glDeleteProgram(this->programId);
}

bool Material::link(const VertexShader& vertexShader, const FragmentShader& fragmentShader)
{
    this->programId = glCreateProgram();
    glAttachShader(this->programId, vertexShader.getShaderId());
    glAttachShader(this->programId, fragmentShader.getShaderId());
    glLinkProgram(this->programId);

    GLint isLinked = 0;
    glGetProgramiv(this->programId, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(this->programId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(this->programId, maxLength, &maxLength, &errorLog[0]);

        std::cerr << std::string("Material : Failed to link Shaders") << std::endl;
        std::cerr << std::string(&errorLog[0]) << std::endl;

        glDeleteProgram(this->programId);
        this->programId = 0;

        return false;
    }

    return true;
}

void Material::use()
{
    glUseProgram(this->programId);

    // Rebind texture

    int offset = 0;

    auto it = this->locationToTextureId.begin();
    auto itEnd = this->locationToTextureId.end();

    while (it != itEnd)
    {
        glUniform1i(it->first, offset);

        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, it->second);

        ++offset;
        ++it;
    }    
}

GLint Material::getLocationFromName(const std::string& name)
{
    auto it = this->nameToLocationMap.find(name);
    if (it == this->nameToLocationMap.end())
    {
        GLint location = glGetUniformLocation(this->programId, name.c_str());

        this->nameToLocationMap.emplace(name, location);

        return location;
    }
    else
    {
        return it->second;
    }
}

void Material::setInt(const std::string& name, int value)
{
    GLint location = getLocationFromName(name);
    glUniform1i(location, value);
}

void Material::setFloat(const std::string& name, float value)
{
    GLint location = getLocationFromName(name);
    glUniform1f(location, value);
}

void Material::setVec4(const std::string& name, const glm::vec4& value)
{
    GLint location = getLocationFromName(name);
    glUniform4fv(location, 1, &value[0]);
}

void Material::setMat4(const std::string& name, const glm::mat4& value)
{
    GLint location = getLocationFromName(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Material::setTexture(const std::string& name, const Texture& value)
{
    GLint location = getLocationFromName(name);
    locationToTextureId[location] = value.getTextureId();
}
