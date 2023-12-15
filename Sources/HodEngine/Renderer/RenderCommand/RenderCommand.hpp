#pragma once

namespace hod
{
	namespace renderer
	{
		class CommandBuffer;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderCommand
		{
		public:

							RenderCommand() = default;
							RenderCommand(const RenderCommand&) = delete;
							RenderCommand(RenderCommand&&) = delete;
			virtual			~RenderCommand() = default;

			void			operator=(const RenderCommand&) = delete;
			void			operator=(RenderCommand&&) = delete;

		public:

			virtual void	Execute(CommandBuffer* commandBuffer) = 0;
		};
	}
}
