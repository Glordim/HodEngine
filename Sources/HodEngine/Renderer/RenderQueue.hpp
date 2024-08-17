#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vector>

namespace hod
{
	namespace renderer
	{
		class RenderCommand;
		class RenderTarget;
		class Context;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API RenderQueue
		{
		public:

			void							PushRenderCommand(RenderCommand* renderCommand);
			void							Execute(RenderTarget* renderTarget = nullptr);
			void							Execute(Context* context);

		private:

			std::vector<RenderCommand*>		_renderCommands;
		};
	}
}
