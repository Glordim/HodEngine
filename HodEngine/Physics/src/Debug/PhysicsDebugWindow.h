#pragma once

#include <DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

namespace HOD
{
    namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class PhysicsDebugWindow : public DEBUG_LAYER::DebugWindow
        {
		public:

						PhysicsDebugWindow();
                        ~PhysicsDebugWindow() override;

            void        Draw() override;
        };
    }
}
