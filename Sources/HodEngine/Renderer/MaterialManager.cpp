#include "HodEngine/Renderer/MaterialManager.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

namespace hod
{
	namespace renderer
	{
		_SingletonConstructor(MaterialManager)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		UID MaterialManager::CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			Renderer* renderer = Renderer::GetInstance();

			Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
			if (vertexShader->LoadFromFile("Shader/" + shaderName + ".vert.spirv") == false)
			{
				delete vertexShader;
				return UID::INVALID_UID;
			}

			Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
			if (fragmentShader->LoadFromFile("Shader/" + shaderName + ".frag.spirv") == false)
			{
				delete vertexShader;
				delete fragmentShader;
				return UID::INVALID_UID;
			}

			Material* material = renderer->CreateMaterial(nullptr, 0, vertexShader, fragmentShader, polygonMode, topololy, useDepth);
			if (material == nullptr)
			{
				delete vertexShader;
				delete fragmentShader;
				return UID::INVALID_UID;
			}

			UID uid = UID::GenerateUID();

			this->AddData(uid, material);

			return uid;
		}
	}
}
