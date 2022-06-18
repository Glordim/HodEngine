#include "Renderer.h"

#include "MaterialManager.h"
#include "HodEngine/Renderer/src/RHI/Texture.h"

namespace hod
{
	template<>
	renderer::Renderer* Singleton<renderer::Renderer>::_instance = nullptr;

	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Renderer::Renderer()
		{
			Renderer::_instance = this;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Renderer::~Renderer()
		{
			Renderer::_instance = nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderQueue* Renderer::GetRenderQueue()
		{
			return &_renderQueue;
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
				_defaultWhiteTexture->BuildBuffer(2, 2, pixels);
			}
			return _defaultWhiteTexture;
		}
	}
}