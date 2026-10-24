#pragma once
#include "HodEngine/RHI/Export.hpp"

#if defined(PLATFORM_MACOS)

	#include "HodEngine/RHI/RhiDevice.hpp"

namespace NS
{
	class Object;
}

namespace MTL
{
	class Buffer;
	class Device;
	class ResidencySet;
	class Allocation;
	class SharedEvent;
}

namespace MTL4
{
	class CommandQueue;
	class CommandAllocator;
	class CommandBuffer;
	class ComputeCommandEncoder;
}

namespace hod::inline rhi
{
	class MetalContext;
	class MetalDevice;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API MetalRhiDevice : public RhiDevice
	{
		_SingletonOverride(MetalRhiDevice)

	protected:
		~MetalRhiDevice() override;

	public:
		bool CreateContext(Window* window); // TODO virtual in Renderer ?

		void WaitIdle() override;

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		PresentationSurface* CreatePresentationSurface(window::Window* window) override;

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr,
									const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) override;

		CommandBuffer*    CreateCommandBuffer() override;
		Buffer*           CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader*           CreateShader(Shader::ShaderType type) override;
		GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipeline::CreateInfo& createInfo) override;
		Texture*          CreateTexture() override;
		RenderTarget*     CreateRenderTarget() override;
		Semaphore*        CreateSemaphore() override;
		Fence*            CreateFence() override;
		BindGroup*        CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
		                                  const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount) override;
		uint32_t          GetUniformBufferOffsetAlignment() const override;

		MTL::Device*        GetDevice() const;
		MTL4::CommandQueue* GetCommandQueue() const;
		MTL4::CommandAllocator* GetCommandAllocator(uint32_t frameIndex) const;

		// Memory the GPU reads for the command buffers of a frame only: push constants, argument buffers, indices made by a draw.
		// Returns where to write, and gives the address the GPU reads it from. Reused once the GPU is done with the frame, see FlushDeferredDeletions.
		void* AllocateTransient(uint32_t frameIndex, uint32_t size, uint64_t& gpuAddress);

		// Taken into account by the next submission: the residency set is committed once, ahead of it
		void AddResourceToResidencySet(const MTL::Allocation* allocation);

		// Released once the GPU is done with the submissions that may use it: the next one, see GetDeferredDeletionSubmissionIndex
		void DeferRelease(NS::Object* object);
		// The same for a buffer or a texture, which stays in the residency set until then
		void DeferReleaseResident(MTL::Allocation* allocation);

		// Commands that upload data into resources (blits), recorded at any time without the CPU waiting for them: they are committed ahead of
		// the next SubmitCommandBuffers, so that what they upload is ready for it. What is released meanwhile is kept until they are done.
		MTL4::ComputeCommandEncoder* GetUploadEncoder();
		// A buffer, in memory shared with the CPU, for an upload command to copy from. Released once the upload commands are done.
		MTL::Buffer* CreateUploadBuffer(uint32_t size);

	protected:
		bool     InitDevice(uint32_t physicalDeviceIdentifier) override;
		Sampler* CreateSampler(const Sampler::CreateInfo& createInfo) override;
		void FlushDeferredDeletions(uint32_t frameIndex) override;

	private:
		// To call after each commit: gives it the next submission index, which the submission event takes once the GPU is done with it
		void SignalSubmissionIndex();
		// What is released now may be used by the submissions up to the next one
		uint64_t GetDeferredDeletionSubmissionIndex() const;
		void     ReleaseCompletedDeletions(uint64_t completedSubmissionIndex);

		void CommitResidencySet();

		// Ends the pending upload commands, for the caller to commit them now. Null when there are none.
		MTL4::CommandBuffer* EndUploads();
		void                 FlushUploads(); // commits them on their own, for WaitIdle

	private:
		// What the command buffers of a frame in flight are recorded with, reused once the GPU is done with them
		struct FrameData
		{
			MTL4::CommandAllocator* _commandAllocator = nullptr;
			uint64_t                _submissionIndex = 0; // the last one with command buffers recorded with it

			Vector<MTL::Buffer*> _transientBuffers;      // filled one after the other
			uint32_t             _transientBufferIndex = 0; // the one being filled
			uint32_t             _transientBufferOffset = 0;
		};

	private:
		MTL::Device*        _device = nullptr;
		MTL4::CommandQueue* _commandQueue = nullptr;
		Vector<FrameData>   _frames; // one per frame in flight
		MTL::ResidencySet*  _residencySet = nullptr;
		bool                _residencySetChanged = false; // not committed yet

		// Takes the index of each submission once the GPU is done with it, and so with the ones before
		MTL::SharedEvent* _submissionEvent = nullptr;
		uint64_t          _lastSubmissionIndex = 0;

		// Released with the index of the next submission: once the GPU is done with it, the last one that may use them
		template<typename T>
		struct Deferred
		{
			T        _object = nullptr;
			uint64_t _submissionIndex = 0;
		};

		Vector<Deferred<NS::Object*>>      _objectsToRelease;
		Vector<Deferred<MTL::Allocation*>> _allocationsToRelease;

		// Recording, until EndUploads. The allocator is the upload command buffer's own: released with it, whatever the frame it is committed with
		MTL4::CommandAllocator*      _uploadCommandAllocator = nullptr;
		MTL4::CommandBuffer*         _uploadCommandBuffer = nullptr;
		MTL4::ComputeCommandEncoder* _uploadEncoder = nullptr;
	};
}

#endif
