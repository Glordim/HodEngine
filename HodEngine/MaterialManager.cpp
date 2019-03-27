#include "MaterialManager.h"

#include "VertexShader.h"
#include "FragmentShader.h"

#include "Material.h"

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

    VertexShader vertexShader;
    if (vertexShader.load("Shader/" + shaderName + ".vert") == false)
        return nullptr;

    FragmentShader fragmentShader;
    if (fragmentShader.load("Shader/" + shaderName + ".frag") == false)
        return nullptr;

    Material* material = new Material();
    if (material->link(vertexShader, fragmentShader) == false)
    {
        delete material;
        return nullptr;
    }

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
