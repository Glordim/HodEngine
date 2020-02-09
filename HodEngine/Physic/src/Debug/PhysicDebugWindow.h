#pragma once

#include <DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

namespace HOD
{
    namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class ActorDebugWindow : public DEBUG_LAYER::DebugWindow
        {
		public:

											ActorDebugWindow();
                                            ~ActorDebugWindow() override;

            void                            Draw() override;

            void                            ShowActor(GAME::Actor* pActor);

        private:

			GAME::Actor*                    pActor = nullptr;
            std::map<const char*, bool>     actorComponentCollapseMap;
        };
    }
}
