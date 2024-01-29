#include "HodEngine/Renderer/MaterialManager.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include "HodEngine/Renderer/Shader/Generated/P2fT2f_Texture_Unlit.vert.hpp"
#include "HodEngine/Renderer/Shader/Generated/P2fT2f_Texture_Unlit.frag.hpp"

namespace hod
{
	namespace renderer
	{
		_SingletonConstructor(MaterialManager)
		{

		}

		struct BuiltinMaterialSource
		{
			BuiltinMaterialSource(void* vertexShaderSource, uint32_t vertexShaderSize, void* fragmentShaderSource, uint32_t fragmentShaderSize)
			: _vertexShaderSource(vertexShaderSource)
			, _vertexShaderSize(vertexShaderSize)
			, _fragmentShaderSource(fragmentShaderSource)
			, _fragmentShaderSize(fragmentShaderSize)
			{

			}

			void*		_vertexShaderSource = nullptr;
			uint32_t	_vertexShaderSize = 0;
			void*		_fragmentShaderSource = nullptr;
			uint32_t	_fragmentShaderSize = 0;
		};

		static std::array<BuiltinMaterialSource, static_cast<uint32_t>(MaterialManager::BuiltinMaterial::Count)> _builtinMaterialSources = {
			BuiltinMaterialSource(P2fT2f_Texture_Unlit_vert, P2fT2f_Texture_Unlit_vert_size, P2fT2f_Texture_Unlit_frag, P2fT2f_Texture_Unlit_frag_size)
		}; // c++23 std::to_underlying 

		/// @brief 
		/// @param buildMaterial 
		/// @return 
		const Material* MaterialManager::GetBuiltinMaterial(BuiltinMaterial buildMaterial)
		{
			assert(buildMaterial != BuiltinMaterial::Count);
			Material* material = _builtinMaterials[static_cast<uint32_t>(buildMaterial)]; // c++23 std::to_underlying 
			if (material == nullptr)
			{
				Renderer* renderer = Renderer::GetInstance();

				const BuiltinMaterialSource& builtinMaterialSource = _builtinMaterialSources[static_cast<uint32_t>(buildMaterial)]; // c++23 std::to_underlying

				Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
				if (vertexShader->LoadFromMemory(builtinMaterialSource._vertexShaderSource, builtinMaterialSource._vertexShaderSize) == false)
				{
					delete vertexShader;
					return nullptr;
				}

				Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
				if (fragmentShader->LoadFromMemory(builtinMaterialSource._fragmentShaderSource, builtinMaterialSource._fragmentShaderSize) == false)
				{
					delete vertexShader;
					delete fragmentShader;
					return nullptr;
				}

				// TODO TODO TODO !!!!!
				renderer::VertexInput vertexInput[2] = {
					{ 0, renderer::VertexInput::Format::R32G32_SFloat },
					{ 8, renderer::VertexInput::Format::R32G32_SFloat },
				};

				material = renderer->CreateMaterial(vertexInput, 2, vertexShader, fragmentShader, Material::PolygonMode::Fill, Material::Topololy::TRIANGLE, true);
				if (material == nullptr)
				{
					delete vertexShader;
					delete fragmentShader;
					return nullptr;
				}

				_builtinMaterials[static_cast<uint32_t>(buildMaterial)] = material; // c++23 std::to_underlying 
			}
			return material;
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
