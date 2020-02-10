#include "DebugWindow.h"

namespace HOD
{
    namespace DEBUG_LAYER
    {
		DebugWindow::DebugWindow()
		{

		}

		DebugWindow::~DebugWindow()
		{

		}

        void DebugWindow::SetVisible(bool bVisible)
        {
			_bIsVisible = bVisible;
        }
    }
}
