#pragma once

#include <DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

namespace HOD
{
    //namespace PHYSIC
	//{
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
    //}
}
