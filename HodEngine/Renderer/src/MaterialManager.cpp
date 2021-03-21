#include "MaterialManager.h"

#include "RHI/Shader.h"
#include "RHI/Material.h"

#include "Renderer.h"

namespace HOD
{
	namespace RENDERER
	{
		MaterialManager* MaterialManager::_instance = nullptr;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialManager* MaterialManager::GetInstance()
		{
			if (MaterialManager::_instance == nullptr)
			{
				MaterialManager::_instance = new MaterialManager();
			}

			return MaterialManager::_instance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialManager::DestroyInstance()
		{
			if (MaterialManager::_instance != nullptr)
			{
				delete MaterialManager::_instance;
				MaterialManager::_instance = nullptr;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialManager::MaterialManager()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialManager::~MaterialManager()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material* MaterialManager::GetMaterial(const std::string& shaderName, Material::Topololy topololy, bool useDepth)
		{
			auto it = _shaderNameToMaterialMap.find(shaderName);
			if (it != _shaderNameToMaterialMap.end() && it->second != nullptr)
			{
				return it->second;
			}

			Renderer* renderer = Renderer::GetInstance();

			Shader* vertexShader = renderer->CreateShader("Shader/" + shaderName + ".vert.spirv", Shader::ShaderType::Vertex);
			if (vertexShader == nullptr)
			{
				return nullptr;
			}

			Shader* fragmentShader = renderer->CreateShader("Shader/" + shaderName + ".frag.spirv", Shader::ShaderType::Fragment);
			if (fragmentShader == nullptr)
			{
				return nullptr;
			}

			Material* material = renderer->CreateMaterial(vertexShader, fragmentShader, topololy, useDepth);
			if (material == nullptr)
			{
				return nullptr;
			}

			_shaderNameToMaterialMap[shaderName] = material;

			return material;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MaterialManager::DestroyMaterial(const std::string& shaderName)
		{
			auto it = _shaderNameToMaterialMap.find(shaderName);
			if (it != _shaderNameToMaterialMap.end())
			{
				if (it->second != nullptr)
				{
					delete it->second;
				}
				_shaderNameToMaterialMap.erase(it);
			}
		}
	}
}
