#include "Renderer.h"

#include <DebugLayer/src/DebugLayer.h>

namespace HOD
{
	RENDERER::Renderer* CORE::Singleton<RENDERER::Renderer>::_instance = nullptr;

	namespace RENDERER
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Renderer::GetVisualizationMode() const
		{
			return _bVisualize3D;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Renderer::SetVisualizationMode(bool bVisualize3D)
		{
			_bVisualize3D = bVisualize3D;
		}
	}
}