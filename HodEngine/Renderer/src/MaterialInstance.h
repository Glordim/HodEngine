#pragma once

#include <map>
#include <string>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

namespace HOD
{
    class Shader;
    class Texture;
    class Material;

    class MaterialInstance
    {
    public:
        MaterialInstance();
        virtual ~MaterialInstance();

        virtual bool SetMaterial(Material* material) = 0;

        virtual void SetInt(const std::string& memberName, int value) = 0;
        virtual void SetFloat(const std::string& memberName, float value) = 0;
        virtual void SetVec4(const std::string& memberName, const glm::vec4& value) = 0;
        virtual void SetMat4(const std::string& memberName, const glm::mat4& value) = 0;
        virtual void SetTexture(const std::string& name, const Texture& value) = 0;
    };
}
