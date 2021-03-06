#include "MaterialManager.h"

#include "RHI/Shader.h"
#include "RHI/Material.h"

#include "Renderer.h"

namespace HOD
{
	template<>
	CORE::GenericManager<RENDERER::Material>* CORE::Singleton<CORE::GenericManager<RENDERER::Material>, RENDERER::MaterialManager>::_instance = nullptr;

	namespace RENDERER
	{
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
		CORE::UID MaterialManager::CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			Renderer* renderer = Renderer::GetInstance();

			Shader* vertexShader = renderer->CreateShader("Shader/" + shaderName + ".vert.spirv", Shader::ShaderType::Vertex);
			if (vertexShader == nullptr)
			{
				return CORE::UID::INVALID_UID;
			}

			Shader* fragmentShader = renderer->CreateShader("Shader/" + shaderName + ".frag.spirv", Shader::ShaderType::Fragment);
			if (fragmentShader == nullptr)
			{
				return CORE::UID::INVALID_UID;
			}

			Material* material = renderer->CreateMaterial(vertexShader, fragmentShader, polygonMode, topololy, useDepth);
			if (material == nullptr)
			{
				return CORE::UID::INVALID_UID;
			}

			CORE::UID uid = CORE::UID::GenerateUID();

			this->AddData(uid, material);

			return uid;
		}
	}
}
