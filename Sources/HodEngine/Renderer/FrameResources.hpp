#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"

#include <HodEngine/Core/Vector.hpp>

namespace hod::renderer
{
	class CommandBuffer;
	class Buffer;
	class Semaphore;
	class Fence;

	class HOD_RENDERER_API FrameResources
	{
	public:
		FrameResources() = default;
		~FrameResources();

		CommandBuffer* CreateCommandBuffer();
		Buffer*        CreateBuffer(Buffer::Usage usage, uint32_t size);
		Semaphore*     CreateSemaphore();
		Fence*         CreateFence();

		void DestroyAll();

	private:
		Vector<CommandBuffer*> _commandBuffers;
		Vector<Buffer*>        _buffers;
		Vector<Semaphore*>     _semaphores;
		Vector<Fence*>         _fences;
	};
}
