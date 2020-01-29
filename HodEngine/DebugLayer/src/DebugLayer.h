#pragma once

#include <Core/src/Singleton.h>

#include <vector>
#include <map>

namespace HOD
{
    namespace GAME
    {
        class Actor;
    }

    namespace DEBUG_LAYER
    {
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

            void                            Draw();

            void                            ShowActor(GAME::Actor* pActor);

        private:

            GAME::Actor*                    pActor = nullptr;
            std::map<const char*, bool>     actorComponentCollapseMap;
        };
    }
}
