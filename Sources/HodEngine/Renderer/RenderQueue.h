#pragma once

#include <vector>

namespace hod
{
	namespace renderer
	{
		class RenderCommand;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderQueue
		{
		public:

			void							PushRenderCommand(RenderCommand* renderCommand);
			void							Execute();

		private:

			std::vector<RenderCommand*>		_renderCommands;
		};
	}
}
