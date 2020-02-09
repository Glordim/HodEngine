#pragma once

#include <Core/src/Singleton.h>

#include <vector>
#include <map>

namespace HOD
{
    namespace DEBUG_LAYER
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class DebugWindow
        {
        public:

							DebugWindow();
			virtual			~DebugWindow();

            virtual void	Draw() = 0;

			void			SetVisible(bool bVisible);

        private:

			bool			_bIsVisible = false;
        };
    }
}
