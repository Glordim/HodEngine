#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/RHI/Buffer.hpp"

#include <HodEngine/Core/Vector.hpp>

namespace hod::inline rhi
{
	class CommandBuffer;
	class Buffer;
	class Semaphore;
	class Fence;
	class PresentationSurface;
}

namespace hod::inline renderer
{
	class RenderView;

	class HOD_RENDERER_API FrameResources
	{
	public:
		FrameResources() = default;
		~FrameResources();

		CommandBuffer* CreateCommandBuffer();
		Buffer*        CreateBuffer(Buffer::Usage usage, uint32_t size);
		Semaphore*     CreateSemaphore();
		Fence*         CreateFence();

		RenderView* CreateRenderView();

		bool       AcquireSurface(PresentationSurface* presentationSurface);
		Semaphore* GetImageAvalaibleSemaphore(PresentationSurface* presentationSurface);

		bool Submit();
		void Wait();
		void DestroyAll();

	private:
		struct ImageAvalaibleSemaphore
		{
			PresentationSurface* _presentationSurface = nullptr;
			Semaphore*           _semaphore = nullptr;
		};

	private:
		Vector<CommandBuffer*> _commandBuffers;
		Vector<Buffer*>        _buffers;
		Vector<Semaphore*>     _semaphores;
		Vector<Fence*>         _fences;

		Vector<RenderView*> _renderViews;

		Vector<ImageAvalaibleSemaphore> _imageAvalaibleSemaphores;
	};
}
