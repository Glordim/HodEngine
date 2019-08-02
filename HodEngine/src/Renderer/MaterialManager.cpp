#include "MaterialManager.h"

#include "Shader.h"

#include "Material.h"

#include "Renderer.h"

MaterialManager* MaterialManager::instance = nullptr;

MaterialManager* MaterialManager::getInstance()
{
    if (MaterialManager::instance == nullptr)
    {
        MaterialManager::instance = new MaterialManager();
    }

    return MaterialManager::instance;
}

void MaterialManager::destroyInstance()
{
    if (MaterialManager::instance != nullptr)
    {
        delete MaterialManager::instance;
        MaterialManager::instance = nullptr;
    }
}

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

Material* MaterialManager::getMaterial(const std::string& shaderName)
{
    auto it = this->shaderNameToMaterialMap.find(shaderName);
    if (it != this->shaderNameToMaterialMap.end() && it->second != nullptr)
    {
        return it->second;
    }

    Renderer* renderer = Renderer::GetInstance();

    Shader* vertexShader = renderer->CreateShader("Shader/" + shaderName + ".vert", Shader::ShaderType::Vertex);
    if (vertexShader == nullptr)
        return nullptr;

    Shader* fragmentShader = renderer->CreateShader("Shader/" + shaderName + ".frag", Shader::ShaderType::Fragment);
    if (fragmentShader == nullptr)
        return nullptr;

    Material* material = renderer->CreateMaterial(vertexShader, fragmentShader);
    if (material == nullptr)
        return nullptr;

    this->shaderNameToMaterialMap[shaderName] = material;

    return material;
}

void MaterialManager::destroyMaterial(const std::string& shaderName)
{
    auto it = this->shaderNameToMaterialMap.find(shaderName);
    if (it != this->shaderNameToMaterialMap.end())
    {
        if (it->second != nullptr)
            delete it->second;
        this->shaderNameToMaterialMap.erase(it);
    }
}
