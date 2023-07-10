#pragma once

#include <vector>

namespace hod
{
	namespace renderer
	{
		class RenderCommand;
		class RenderTarget;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderQueue
		{
		public:

			void							PushRenderCommand(RenderCommand* renderCommand);
			void							Execute(RenderTarget* renderTarget = nullptr);

		private:

			std::vector<RenderCommand*>		_renderCommands;
		};
	}
}
