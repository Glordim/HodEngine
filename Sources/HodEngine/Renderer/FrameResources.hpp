#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/Renderer/UniformAllocator.hpp"

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
	class MaterialInstance;
	class RenderView;

	class HOD_RENDERER_API FrameResources
	{
	public:
		FrameResources() = default;
		FrameResources(FrameResources&&) = default;
		~FrameResources();

		CommandBuffer* CreateCommandBuffer();
		Buffer*        CreateBuffer(Buffer::Usage usage, uint32_t size);
		Semaphore*     CreateSemaphore();
		Fence*         CreateFence();

		RenderView* CreateRenderView();

		// Uniform data of this frame, see MaterialInstance::Bind
		UniformAllocator& GetUniformAllocator();

		// Deletes a MaterialInstance made for this frame only, once the frame is done
		void DeleteAfter(MaterialInstance* materialInstance);

		bool       AcquireSurface(PresentationSurface* presentationSurface);
		Semaphore* GetImageAvalaibleSemaphore(PresentationSurface* presentationSurface);

		bool Submit();
		void Wait();
		void DestroyAll();

		// DestroyAll, and releases what is otherwise kept from one frame to the next
		void Clear();

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

		Vector<MaterialInstance*> _materialInstances;
		UniformAllocator          _uniformAllocator;

		Vector<ImageAvalaibleSemaphore> _imageAvalaibleSemaphores;
	};
}
