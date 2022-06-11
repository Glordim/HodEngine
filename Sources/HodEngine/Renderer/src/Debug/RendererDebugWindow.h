#pragma once

#include <HodEngine/DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererDebugWindow : public DEBUG_LAYER::DebugWindow
		{
		public:

			RendererDebugWindow();
			~RendererDebugWindow() override;

			void        Draw() override;
		};
	}
}
