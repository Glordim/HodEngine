#pragma once

#include <Core/src/Singleton.h>

#include <vector>
#include <map>

namespace HOD
{
    namespace GAME
    {
        class Actor;
		class CameraComponent;
    }

    namespace DEBUG_LAYER
    {
		class DebugWindow;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class DebugLayer : public CORE::Singleton<DebugLayer>
        {
            friend class CORE::Singleton<DebugLayer>;

        protected:

                                            DebugLayer();
                                            ~DebugLayer() override;

        public:

            bool                            Init();
            void                            Clear();

			void							RegisterDebugWindow(DebugWindow* pDebugWindow);
			void							UnregisterDebugWindow(DebugWindow* pDebugWindow);

            void                            Draw();

        private:

			std::vector<DebugWindow*>		_vDebugWindows;
        };
    }
}
