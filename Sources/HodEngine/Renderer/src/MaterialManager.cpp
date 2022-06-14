#include "MaterialManager.h"

#include "RHI/Shader.h"
#include "RHI/Material.h"

#include "Renderer.h"

namespace HOD
{
	namespace RENDERER
	{
		_SingletonInstance(MaterialManager)

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		CORE::UID MaterialManager::CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			Renderer* renderer = Renderer::GetInstance();

			Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
			if (vertexShader->LoadFromFile("Shader/" + shaderName + ".vert.spirv") == false)
			{
				delete vertexShader;
				return CORE::UID::INVALID_UID;
			}

			Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
			if (fragmentShader->LoadFromFile("Shader/" + shaderName + ".frag.spirv") == false)
			{
				delete vertexShader;
				delete fragmentShader;
				return CORE::UID::INVALID_UID;
			}

			Material* material = renderer->CreateMaterial(vertexShader, fragmentShader, polygonMode, topololy, useDepth);
			if (material == nullptr)
			{
				delete vertexShader;
				delete fragmentShader;
				return CORE::UID::INVALID_UID;
			}

			CORE::UID uid = CORE::UID::GenerateUID();

			this->AddData(uid, material);

			return uid;
		}
	}
}
