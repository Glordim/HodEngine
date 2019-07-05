#ifndef __MATERIAL_INSTANCE_HPP__
#define __MATERIAL_INSTANCE_HPP__

#include <map>
#include <string>

#include "glad/glad.h"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class Shader;
class Texture;
class Material;

class MaterialInstance
{
public:
    MaterialInstance();
    virtual ~MaterialInstance();

    virtual bool SetMaterial(Material* material) = 0;

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    void setTexture(const std::string& name, const Texture& value);
};

#endif
