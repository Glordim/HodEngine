#pragma once

#include <DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

namespace HOD
{
	namespace GAME
	{
		class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ActorDebugWindow : public DEBUG_LAYER::DebugWindow
		{
		public:

										ActorDebugWindow();
										ActorDebugWindow(const ActorDebugWindow&) = delete;
										ActorDebugWindow(ActorDebugWindow&&) = delete;
										~ActorDebugWindow() override;

			void						operator=(const ActorDebugWindow&) = delete;
			void						operator=(ActorDebugWindow&&) = delete;

		public:

			void						Draw() override;

			void						SetActor(Actor* actor);

		private:

			Actor*						_actor = nullptr;
			std::map<const char*, bool>	_actorComponentCollapseMap;
		};
	}
}
