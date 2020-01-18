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

    class Material
    {
    public:
        enum Topololy
        {
            POINT,
            LINE,
            TRIANGLE
        };

        Material();
        virtual ~Material();

        virtual bool Build(Shader* vertexShader, Shader* fragmentShader, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) = 0;

        bool link(Shader* vertexShader, Shader* fragmentShader);
        void use();

    private:

        uint32_t getLocationFromName(const std::string& name);

        std::map<std::string, uint32_t> nameToLocationMap;
        std::map<uint32_t, uint32_t> locationToTextureId;

        uint32_t programId;
    };
}
