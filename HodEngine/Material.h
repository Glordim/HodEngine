#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include <map>
#include <string>

#include "glad/glad.h"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class VertexShader;
class FragmentShader;
class Texture;

class Material
{
public:
    Material();
    virtual ~Material();

    bool link(const VertexShader& vertexShader, const FragmentShader& fragmentShader);
    void use();

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    void setTexture(const std::string& name, const Texture& value);

private:

    GLint getLocationFromName(const std::string& name);

    std::map<std::string, GLint> nameToLocationMap;
    std::map<GLint, GLint> locationToTextureId;

    GLuint programId;
};

#endif
