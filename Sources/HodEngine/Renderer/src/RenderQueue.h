#pragma once

#include <vector>

namespace HOD
{
	namespace RENDERER
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
