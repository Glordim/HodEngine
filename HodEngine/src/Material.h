#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include <map>
#include <string>

#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

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

    virtual bool Build(Shader* vertexShader, Shader* fragmentShader, Topololy topololy = Topololy::TRIANGLE) = 0;

    bool link(Shader* vertexShader, Shader* fragmentShader);
    void use();

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    void setTexture(const std::string& name, const Texture& value);

private:

    uint32_t getLocationFromName(const std::string& name);

    std::map<std::string, uint32_t> nameToLocationMap;
    std::map<uint32_t, uint32_t> locationToTextureId;

    uint32_t programId;
};

#endif
