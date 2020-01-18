#pragma once

#include <string>
#include <unordered_map>

namespace HOD
{
    class Material;

    class MaterialManager
    {
    public:
        static MaterialManager* getInstance();
        static void destroyInstance();

        Material* getMaterial(const std::string& shaderName, bool useDepth = true);
        void destroyMaterial(const std::string& shaderName);

    private:
        MaterialManager();
        MaterialManager(const MaterialManager& copy) = delete;
        void operator=(const MaterialManager& right) = delete;
        virtual ~MaterialManager();

        static MaterialManager* instance;

        std::unordered_map<std::string, Material*> shaderNameToMaterialMap;
    };
}
