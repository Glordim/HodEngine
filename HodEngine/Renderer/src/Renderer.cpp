#include "Renderer.h"

#include <DebugLayer/src/DebugLayer.h>

namespace HOD
{
    Renderer* CORE::Singleton<Renderer>::_instance = nullptr;

    Renderer::Renderer()
    {
        Renderer::_instance = this;
    }

    Renderer::~Renderer()
    {
        Renderer::_instance = nullptr;
    }

	bool Renderer::Init()
	{
		DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

		pDebugLayer->RegisterDebugWindow(&_rendererDebugWindow);

		return true;
	}

	void Renderer::Clear()
	{
		DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

		pDebugLayer->UnregisterDebugWindow(&_rendererDebugWindow);
	}

	bool Renderer::GetVisualizationMode() const
	{
		return _bVisualize3D;
	}

	void Renderer::SetVisualizationMode(bool bVisualize3D)
	{
		_bVisualize3D = bVisualize3D;
	}
}
