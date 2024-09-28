#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Renderer/PickingManager.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Renderer/Shader/Generated/SpriteUnlitColor.vert.hpp"
#include "HodEngine/Renderer/Shader/Generated/SpriteUnlitColor.frag.hpp"

#include "HodEngine/Renderer/RHI/VertexInput.hpp"

namespace hod
{
	namespace renderer
	{
		/// @brief 
		_SingletonConstructor(Renderer)
		{
			MaterialManager::CreateInstance();
			PickingManager::CreateInstance();
			RenderQueue::CreateInstance();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Renderer::~Renderer()
		{
			RenderQueue::DestroyInstance();
			PickingManager::DestroyInstance();
			MaterialManager::DestroyInstance();
		}

		/// @brief 
		void Renderer::Clear()
		{
			RenderQueue::GetInstance()->Terminate();
			MaterialManager::GetInstance()->Clear();

			delete _overdrawnMaterialInstance;
			_overdrawnMaterialInstance = nullptr;
			delete _overdrawnMaterial;
			_overdrawnMaterial = nullptr;

			delete _wireframeMaterialInstance;
			_wireframeMaterialInstance = nullptr;
			delete _wireframeMaterial;
			_wireframeMaterial = nullptr;

			delete _defaultMaterialInstance;
			_defaultMaterialInstance = nullptr;
			delete _defaultMaterial;
			_defaultMaterial = nullptr;
		}

		/*
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Renderer::Init()
		{
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

			pDebugLayer->RegisterDebugWindow(&_rendererDebugWindow);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Renderer::Clear()
		{
			DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

			pDebugLayer->UnregisterDebugWindow(&_rendererDebugWindow);
		}
		*/
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Renderer::VisualizationMode Renderer::GetVisualizationMode() const
		{
			return _visualizationMode;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Renderer::SetVisualizationMode(VisualizationMode visualizationMode)
		{
			_visualizationMode = visualizationMode;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* Renderer::GetDefaultMaterialInstance()
		{
			if (_defaultMaterialInstance == nullptr)
			{
				if (_defaultMaterial == nullptr)
				{
					Renderer* renderer = Renderer::GetInstance();

					renderer::VertexInput vertexInput[1] = {
						{ 0, 0, renderer::VertexInput::Format::R32G32_SFloat },
						//{ 8, renderer::VertexInput::Format::R32G32_SFloat },
						//{ 16, renderer::VertexInput::Format::A8B8G8R8_UNorm_Pack32 },
					};

					Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
					if (vertexShader->LoadFromMemory(SpriteUnlitColor_vert, SpriteUnlitColor_vert_size) == false)
					{
						delete vertexShader;
						return nullptr;
					}

					Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
					if (fragmentShader->LoadFromMemory(SpriteUnlitColor_frag, SpriteUnlitColor_frag_size) == false)
					{
						delete vertexShader;
						delete fragmentShader;
						return nullptr;
					}

					_defaultMaterial = renderer->CreateMaterial(vertexInput, 1, vertexShader, fragmentShader);
					if (_defaultMaterial == nullptr)
					{
						delete vertexShader;
						delete fragmentShader;
						return nullptr;
					}
				}

				_defaultMaterialInstance = CreateMaterialInstance(_defaultMaterial);
			}

			return _defaultMaterialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* Renderer::GetOverdrawMaterialInstance()
		{
			if (_overdrawnMaterialInstance == nullptr)
			{
				if (_overdrawnMaterial == nullptr)
				{
					_overdrawnMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteOverdraw"));
				}

				_overdrawnMaterialInstance = CreateMaterialInstance(_overdrawnMaterial);
			}

			return _overdrawnMaterialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* Renderer::GetWireframeMaterialInstance()
		{
			if (_wireframeMaterialInstance == nullptr)
			{
				if (_wireframeMaterial == nullptr)
				{
					_wireframeMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteWireframe", Material::PolygonMode::Line, Material::Topololy::TRIANGLE));
				}

				_wireframeMaterialInstance = CreateMaterialInstance(_wireframeMaterial);
			}

			return _wireframeMaterialInstance;
		}

		/// @brief 
		/// @return 
		Texture* Renderer::GetDefaultWhiteTexture()
		{
			if (_defaultWhiteTexture == nullptr)
			{
				uint8_t pixels[4*2*2] = { 255, 255, 255, 255,
										  255, 255, 255, 255,
										  255, 255, 255, 255,
										  255, 255, 255, 255 };

				_defaultWhiteTexture = CreateTexture();
				_defaultWhiteTexture->BuildBuffer(2, 2, pixels, Texture::CreateInfo());
			}
			return _defaultWhiteTexture;
		}
	}
}