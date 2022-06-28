#include "MaterialManager.h"

#include "RHI/Shader.h"
#include "RHI/Material.h"

#include "Renderer.h"

namespace hod
{
	namespace renderer
	{
		_SingletonInstance(MaterialManager)

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
