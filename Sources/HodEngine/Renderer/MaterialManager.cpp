#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"

#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/ShaderGenerator/ShaderGenerator.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include "HodEngine/Renderer/Shader/P2fT2f_Texture_Unlit.vert.hpp"
#include "HodEngine/Renderer/Shader/P2fT2f_Texture_Unlit.frag.hpp"
#include "HodEngine/Renderer/Shader/TextureUnlitColor.frag.hpp"

#include "HodEngine/Renderer/Shader/P2fC4f_Unlit.vert.hpp"
#include "HodEngine/Renderer/Shader/P2fC4f_Unlit.frag.hpp"

#include "HodEngine/Renderer/Shader/P2f_Unlit.vert.hpp"
#include "HodEngine/Renderer/Shader/P2f_Unlit.frag.hpp"

#include <span>
#include <cstring>
#include "HodEngine/Core/Vector.hpp"

namespace hod
{
	namespace renderer
	{
		_SingletonConstructor(MaterialManager)
		{

		}

		/// @brief 
		MaterialManager::~MaterialManager()
		{
			Clear();
		}

		/// @brief 
		void MaterialManager::Clear()
		{
			for (uint32_t index = 0; index < _builtinMaterials.size(); ++index)
			{
				DefaultAllocator::GetInstance().Delete(_builtinMaterials[index]);
				_builtinMaterials[index] = nullptr;

				DefaultAllocator::GetInstance().Delete(_builtinVertexShaders[index]);
				_builtinVertexShaders[index] = nullptr;

				DefaultAllocator::GetInstance().Delete(_builtinFragmentShaders[index]);
				_builtinFragmentShaders[index] = nullptr;
			}
		}

		struct BuiltinMaterialSource
		{
			BuiltinMaterialSource(std::string_view vertexShaderSource,
								  std::string_view fragmentShaderSource,
								  const Vector<VertexInput>& vertexInputs,
								  Material::PolygonMode polygonMode, Material::Topololy topology)
			: _vertexShaderSource(vertexShaderSource)
			, _fragmentShaderSource(fragmentShaderSource)
			//, _vertexInput(vertexInput)
			, _polygonMode(polygonMode)
			, _topology(topology)
			{
				_vertexInputs.resize(vertexInputs.size());
				std::memcpy(_vertexInputs.data(), vertexInputs.data(), vertexInputs.size() * sizeof(VertexInput));
			}

			std::string_view			_vertexShaderSource;
			std::string_view			_fragmentShaderSource;

			Vector<VertexInput>	_vertexInputs;

			Material::PolygonMode		_polygonMode;
			Material::Topololy			_topology;

		};

		/// @brief 
		/// @param buildMaterial 
		/// @return 
		const BuiltinMaterialSource& GetBuiltinMaterialSource(MaterialManager::BuiltinMaterial buildMaterial)
		{
			static BuiltinMaterialSource P2f_Unlit_Line(P2f_Unlit_vert, P2f_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Fill, Material::Topololy::LINE);

			static BuiltinMaterialSource P2f_Unlit_Triangle(P2f_Unlit_vert, P2f_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Fill, Material::Topololy::TRIANGLE);

			static BuiltinMaterialSource P2f_Unlit_TriangleFan(P2f_Unlit_vert, P2f_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Fill, Material::Topololy::TRIANGLE_FAN);

			static BuiltinMaterialSource P2f_Unlit_Line_TriangleFan(P2f_Unlit_vert, P2f_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Line, Material::Topololy::TRIANGLE_FAN);

			static BuiltinMaterialSource P2f_Unlit_Line_LineStrip(P2f_Unlit_vert, P2f_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Line, Material::Topololy::LINE_STRIP);

			static BuiltinMaterialSource P2fT2f_Texture_Unlit(P2fT2f_Texture_Unlit_vert, P2fT2f_Texture_Unlit_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Fill, Material::Topololy::TRIANGLE);

			static BuiltinMaterialSource P2fT2f_Texture_Unlit_Color(P2fT2f_Texture_Unlit_vert, TextureUnlitColor_frag,
															  { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32_SFloat) },
															  Material::PolygonMode::Fill, Material::Topololy::TRIANGLE);

			static BuiltinMaterialSource P2fC4f_Unlit_Fill_Triangle(P2fC4f_Unlit_vert, P2fC4f_Unlit_frag,
								  									{ VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32B32A32_SFloat) },
								  									Material::PolygonMode::Fill, Material::Topololy::TRIANGLE);

			static BuiltinMaterialSource P2fC4f_Unlit_Fill_TriangleFan(P2fC4f_Unlit_vert, P2fC4f_Unlit_frag,
								  									   { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32B32A32_SFloat) },
								  									   Material::PolygonMode::Fill, Material::Topololy::TRIANGLE_FAN);

			static BuiltinMaterialSource P2fC4f_Unlit_Line_TriangleFan(P2fC4f_Unlit_vert, P2fC4f_Unlit_frag,
								  									   { VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32B32A32_SFloat) },
								  									   Material::PolygonMode::Line, Material::Topololy::TRIANGLE_FAN);

			static BuiltinMaterialSource P2fC4f_Unlit_Line_Line(P2fC4f_Unlit_vert, P2fC4f_Unlit_frag,
								  								{ VertexInput(0, 0, VertexInput::Format::R32G32_SFloat), VertexInput(1, 8, VertexInput::Format::R32G32B32A32_SFloat) },
								  								Material::PolygonMode::Line, Material::Topololy::LINE);

			static std::array<const BuiltinMaterialSource*, std::to_underlying(MaterialManager::BuiltinMaterial::Count)> _builtinMaterialSources = {
				&P2f_Unlit_Line, 
				&P2f_Unlit_Triangle,
				&P2f_Unlit_TriangleFan,
				&P2f_Unlit_Line_TriangleFan,
				&P2f_Unlit_Line_LineStrip,
				&P2fT2f_Texture_Unlit,
				&P2fT2f_Texture_Unlit_Color,
				&P2fC4f_Unlit_Fill_Triangle,
				&P2fC4f_Unlit_Fill_TriangleFan,
				&P2fC4f_Unlit_Line_TriangleFan,
				&P2fC4f_Unlit_Line_Line,
			};

 			return *_builtinMaterialSources[static_cast<uint32_t>(buildMaterial)]; // c++23 std::to_underlying
		}

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

				const BuiltinMaterialSource& builtinMaterialSource = GetBuiltinMaterialSource(buildMaterial);

				Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
				if (vertexShader->LoadFromSource(builtinMaterialSource._vertexShaderSource) == false)
				{
					DefaultAllocator::GetInstance().Delete(vertexShader);
					return nullptr;
				}

				Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
				if (fragmentShader->LoadFromSource(builtinMaterialSource._fragmentShaderSource) == false)
				{
					DefaultAllocator::GetInstance().Delete(vertexShader);
					DefaultAllocator::GetInstance().Delete(fragmentShader);
					return nullptr;
				}

				material = renderer->CreateMaterial(builtinMaterialSource._vertexInputs.data(), (uint32_t)builtinMaterialSource._vertexInputs.size(), vertexShader, fragmentShader, builtinMaterialSource._polygonMode, builtinMaterialSource._topology, true);
				
				if (material == nullptr)
				{
					DefaultAllocator::GetInstance().Delete(vertexShader);
					DefaultAllocator::GetInstance().Delete(fragmentShader);
					return nullptr;
				}

				_builtinMaterials[static_cast<uint32_t>(buildMaterial)] = material; // c++23 std::to_underlying
				_builtinVertexShaders[static_cast<uint32_t>(buildMaterial)] = vertexShader;
				_builtinFragmentShaders[static_cast<uint32_t>(buildMaterial)] = fragmentShader;

				material->CreateDefaultInstance();
			}
			return material;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		UID MaterialManager::CreateMaterial(const String& shaderName, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			return UID::INVALID_UID; // TODO ?
			
			/*
			Renderer* renderer = Renderer::GetInstance();

			Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
			if (vertexShader->LoadFromFile("Shader/" + shaderName + ".vert.spirv") == false)
			{
				DefaultAllocator::GetInstance().Delete(vertexShader);
				return UID::INVALID_UID;
			}

			Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
			if (fragmentShader->LoadFromFile("Shader/" + shaderName + ".frag.spirv") == false)
			{
				DefaultAllocator::GetInstance().Delete(vertexShader);
				DefaultAllocator::GetInstance().Delete(fragmentShader);
				return UID::INVALID_UID;
			}

			Material* material = renderer->CreateMaterial(nullptr, 0, vertexShader, fragmentShader, polygonMode, topololy, useDepth);
			if (material == nullptr)
			{
				DefaultAllocator::GetInstance().Delete(vertexShader);
				DefaultAllocator::GetInstance().Delete(fragmentShader);
				return UID::INVALID_UID;
			}

			UID uid = UID::GenerateUID();

			this->AddData(uid, material);

			return uid;
			*/
		}
	}
}
