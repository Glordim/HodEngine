#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include "HodEngine/Core/Math.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/RHI/Metal/MetalBindGroup.hpp"
#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalFence.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/Metal/MetalGraphicsPipeline.hpp"
#include "HodEngine/RHI/Metal/MetalSampler.hpp"
#include "HodEngine/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/RHI/Metal/MetalShader.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	namespace
	{
		/// @brief The objects the GPU is done with: the first ones, as their submission indices only grow
		template<typename Deferred>
		uint32_t CountCompleted(const Vector<Deferred>& objects, uint64_t completedSubmissionIndex)
		{
			uint32_t completedCount = 0;
			while (completedCount < objects.Size() && objects[completedCount]._submissionIndex <= completedSubmissionIndex)
			{
				++completedCount;
			}
			return completedCount;
		}

		/// @brief The ones left are shifted to the front, as on Vulkan: only the ones still in flight, once per frame
		template<typename Deferred>
		void EraseFirst(Vector<Deferred>& objects, uint32_t count)
		{
			if (count == 0)
			{
				return;
			}

			for (uint32_t index = count; index < objects.Size(); ++index)
			{
				objects[index - count] = objects[index];
			}
			objects.Resize(objects.Size() - count);
		}
	}

	_SingletonOverrideConstructor(MetalRhiDevice)
	: RhiDevice()
	{
	}

	/// @brief
	MetalRhiDevice::~MetalRhiDevice()
	{
		WaitIdle();

		for (FrameData& frame : _frames)
		{
			frame._commandAllocator->release();
			for (MTL::Buffer* transientBuffer : frame._transientBuffers)
			{
				DeferReleaseResident(transientBuffer);
			}
		}
		_frames.Clear();

		// Nothing is submitted anymore: what was released after the last submission can go too
		ReleaseCompletedDeletions(UINT64_MAX);

		_submissionEvent->release();
		_residencySet->release();
		_commandQueue->release();
		_device->release();
	}

	/// @brief Blocks until the GPU is done with everything submitted so far
	void MetalRhiDevice::WaitIdle()
	{
		if (_commandQueue == nullptr)
		{
			return;
		}

		SignalSubmissionIndex();
		_submissionEvent->waitUntilSignaledValue(_lastSubmissionIndex, UINT64_MAX);

		ReleaseCompletedDeletions(_lastSubmissionIndex);
	}

	/// @brief
	void MetalRhiDevice::SignalSubmissionIndex()
	{
		++_lastSubmissionIndex;
		_commandQueue->signalEvent(_submissionEvent, _lastSubmissionIndex);
	}

	/// @brief
	/// @return
	uint64_t MetalRhiDevice::GetDeferredDeletionSubmissionIndex() const
	{
		return _lastSubmissionIndex + 1;
	}

	/// @brief
	/// @param object
	void MetalRhiDevice::DeferRelease(NS::Object* object)
	{
		if (object != nullptr)
		{
			_objectsToRelease.PushBack({object, GetDeferredDeletionSubmissionIndex()});
		}
	}

	/// @brief
	/// @param allocation
	void MetalRhiDevice::DeferReleaseResident(MTL::Allocation* allocation)
	{
		if (allocation != nullptr)
		{
			_allocationsToRelease.PushBack({allocation, GetDeferredDeletionSubmissionIndex()});
		}
	}

	/// @brief
	/// @param completedSubmissionIndex the GPU is done with the submissions up to it
	void MetalRhiDevice::ReleaseCompletedDeletions(uint64_t completedSubmissionIndex)
	{
		uint32_t completedObjectCount = CountCompleted(_objectsToRelease, completedSubmissionIndex);
		for (uint32_t index = 0; index < completedObjectCount; ++index)
		{
			_objectsToRelease[index]._object->release();
		}
		EraseFirst(_objectsToRelease, completedObjectCount);

		// Out of the residency set, committed once for all of them, before they are released
		uint32_t completedAllocationCount = CountCompleted(_allocationsToRelease, completedSubmissionIndex);
		if (completedAllocationCount > 0)
		{
			for (uint32_t index = 0; index < completedAllocationCount; ++index)
			{
				_residencySet->removeAllocation(_allocationsToRelease[index]._object);
			}
			_residencySetChanged = true;
			CommitResidencySet();

			for (uint32_t index = 0; index < completedAllocationCount; ++index)
			{
				_allocationsToRelease[index]._object->release();
			}
			EraseFirst(_allocationsToRelease, completedAllocationCount);
		}
	}

	bool MetalRhiDevice::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore,
												const Fence* fence)
	{
		MTL4::CommandBuffer** mtlCommandBuffers = (MTL4::CommandBuffer**)alloca(sizeof(MTL4::CommandBuffer*) * commandBufferCount);
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			MetalCommandBuffer* commandBuffer = static_cast<MetalCommandBuffer*>(commandBuffers[commandBufferIndex]);
			mtlCommandBuffers[commandBufferIndex] = commandBuffer->GetNativeCommandBuffer();
		}

		if (waitSemaphore != nullptr)
		{
			const MetalSemaphore* metalSemaphore = static_cast<const MetalSemaphore*>(waitSemaphore);
			_commandQueue->wait(metalSemaphore->GetNativeSemaphore(), metalSemaphore->GetTargetValue());
		}

		// What was added since the previous submission is resident for this one
		CommitResidencySet();

		_commandQueue->commit(mtlCommandBuffers, commandBufferCount);
		SignalSubmissionIndex();

		// Their command allocators and transient memory are in use until this submission is done
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			_frames[static_cast<MetalCommandBuffer*>(commandBuffers[commandBufferIndex])->GetFrameIndex()]._submissionIndex = _lastSubmissionIndex;
		}

		if (signalSemaphore != nullptr)
		{
			MetalSemaphore* metalSemaphore = static_cast<MetalSemaphore*>(const_cast<Semaphore*>(signalSemaphore));
			_commandQueue->signalEvent(metalSemaphore->GetNativeSemaphore(), metalSemaphore->IncrementAndGetTargetValue());
		}

		if (fence != nullptr)
		{
			const MetalFence* metalFence = static_cast<const MetalFence*>(fence);
			_commandQueue->signalEvent(metalFence->GetNativeEvent(), metalFence->GetTargetValue());
		}

		return true;
	}

	CommandBuffer* MetalRhiDevice::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<MetalCommandBuffer>();
	}

	Buffer* MetalRhiDevice::CreateBuffer(Buffer::Usage usage, uint32_t Size)
	{
		return DefaultAllocator::GetInstance().New<MetalBuffer>(usage, Size);
	}

	RenderTarget* MetalRhiDevice::CreateRenderTarget()
	{
		return DefaultAllocator::GetInstance().New<RenderTarget>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalRhiDevice::InitDevice(uint32_t physicalDeviceIdentifier)
	{
		(void)physicalDeviceIdentifier; // TODO

		_device = MTL::CreateSystemDefaultDevice();

		if (_device->supportsFamily(MTL::GPUFamilyMetal4) == false)
		{
			OUTPUT_ERROR("Metal: This GPU does not support Metal 4");
			return false;
		}

		_commandQueue = _device->newMTL4CommandQueue();

		_frames.Resize(GetFrameInFlightCount());
		for (FrameData& frame : _frames)
		{
			frame._commandAllocator = _device->newCommandAllocator();
		}

		MTL::ResidencySetDescriptor* residencySetDescriptor = MTL::ResidencySetDescriptor::alloc()->init();
		NS::Error*                   residencySetError = nullptr;
		_residencySet = _device->newResidencySet(residencySetDescriptor, &residencySetError);
		residencySetDescriptor->release();
		if (_residencySet == nullptr)
		{
			OUTPUT_ERROR("Metal: Unable to create residency set: {}", residencySetError->localizedDescription()->utf8String());
			return false;
		}
		_commandQueue->addResidencySet(_residencySet);

		_submissionEvent = _device->newSharedEvent();

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalRhiDevice::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		(void)availableDevices; // TODO
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	PresentationSurface* MetalRhiDevice::CreatePresentationSurface(window::Window* window)
	{
		MetalPresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<MetalPresentationSurface>(static_cast<MacOsWindow*>(window));
		presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		_presentationSurfaces.PushBack(presentationSurface);
		return presentationSurface;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Shader* MetalRhiDevice::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<MetalShader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline* MetalRhiDevice::CreateGraphicsPipeline(const GraphicsPipeline::CreateInfo& createInfo)
	{
		MetalGraphicsPipeline* graphicsPipeline = DefaultAllocator::GetInstance().New<MetalGraphicsPipeline>();
		if (graphicsPipeline->Build(createInfo) == false)
		{
			DefaultAllocator::GetInstance().Delete(graphicsPipeline);
			return nullptr;
		}
		return graphicsPipeline;
	}

	/// @brief
	/// @return
	Semaphore* MetalRhiDevice::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<MetalSemaphore>();
	}

	/// @brief
	/// @return
	Fence* MetalRhiDevice::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<MetalFence>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Texture* MetalRhiDevice::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<MetalTexture>();
	}

	MTL::Device* MetalRhiDevice::GetDevice() const
	{
		return _device;
	}

	MTL4::CommandQueue* MetalRhiDevice::GetCommandQueue() const
	{
		return _commandQueue;
	}

	MTL4::CommandAllocator* MetalRhiDevice::GetCommandAllocator(uint32_t frameIndex) const
	{
		return _frames[frameIndex]._commandAllocator;
	}

	/// @brief The buffers of the frame are kept from one frame to the next: only a frame needing more than ever before makes a new one
	/// @param frameIndex the one the command buffer is recorded for
	/// @param size
	/// @param gpuAddress
	/// @return
	void* MetalRhiDevice::AllocateTransient(uint32_t frameIndex, uint32_t size, uint64_t& gpuAddress)
	{
		constexpr uint32_t alignment = 16;
		constexpr uint32_t minBufferSize = 256 * 1024;

		size = (size + alignment - 1) & ~(alignment - 1);

		auto newTransientBuffer = [this, size]()
		{
			MTL::Buffer* transientBuffer = _device->newBuffer(Max(size, minBufferSize), MTL::ResourceStorageModeShared);
			AddResourceToResidencySet(transientBuffer);
			return transientBuffer;
		};

		FrameData& frame = _frames[frameIndex];
		while (true)
		{
			if (frame._transientBufferIndex == frame._transientBuffers.Size())
			{
				frame._transientBuffers.PushBack(newTransientBuffer());
				frame._transientBufferOffset = 0;
				break;
			}

			MTL::Buffer* transientBuffer = frame._transientBuffers[frame._transientBufferIndex];
			if (frame._transientBufferOffset + size <= transientBuffer->length())
			{
				break;
			}

			if (frame._transientBufferOffset == 0)
			{
				// Too small for it even empty: replaced by one large enough, the previous one kept until the frames in flight are done with it
				DeferReleaseResident(transientBuffer);
				frame._transientBuffers[frame._transientBufferIndex] = newTransientBuffer();
				break;
			}

			++frame._transientBufferIndex;
			frame._transientBufferOffset = 0;
		}

		MTL::Buffer* transientBuffer = frame._transientBuffers[frame._transientBufferIndex];

		void* data = static_cast<uint8_t*>(transientBuffer->contents()) + frame._transientBufferOffset;
		gpuAddress = transientBuffer->gpuAddress() + frame._transientBufferOffset;
		frame._transientBufferOffset += size;
		return data;
	}

	/// @brief
	/// @param allocation
	void MetalRhiDevice::AddResourceToResidencySet(const MTL::Allocation* allocation)
	{
		_residencySet->addAllocation(allocation);
		_residencySetChanged = true;
	}

	/// @brief
	void MetalRhiDevice::CommitResidencySet()
	{
		if (_residencySetChanged)
		{
			_residencySet->commit();
			_residencySetChanged = false;
		}
	}

	/// @brief
	BindGroup* MetalRhiDevice::CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                                           const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		if (graphicsPipeline == nullptr)
		{
			return nullptr;
		}

		MetalBindGroup* bindGroup = DefaultAllocator::GetInstance().New<MetalBindGroup>();
		if (bindGroup->Build(*static_cast<const MetalGraphicsPipeline*>(graphicsPipeline), set, uniformBuffers, uniformBufferCount, textureBindings, textureBindingCount) ==
		    false)
		{
			DefaultAllocator::GetInstance().Delete(bindGroup);
			return nullptr;
		}
		return bindGroup;
	}

	/// @brief
	Sampler* MetalRhiDevice::CreateSampler(const Sampler::CreateInfo& createInfo)
	{
		MetalSampler* sampler = DefaultAllocator::GetInstance().New<MetalSampler>(createInfo);
		if (sampler->Build() == false)
		{
			DefaultAllocator::GetInstance().Delete(sampler);
			return nullptr;
		}
		return sampler;
	}

	/// @brief
	uint32_t MetalRhiDevice::GetUniformBufferOffsetAlignment() const
	{
		return 256; // TODO
	}

	/// @brief What is released is tagged with a submission index, whatever the frame.
	/// The command allocator and the transient memory of the frame are reused once the GPU is done with the last submission recorded with them:
	/// the Renderer waits for the RenderViews of the frame before, so it is usually done already.
	void MetalRhiDevice::FlushDeferredDeletions(uint32_t frameIndex)
	{
		FrameData& frame = _frames[frameIndex];
		if (_submissionEvent->signaledValue() < frame._submissionIndex)
		{
			_submissionEvent->waitUntilSignaledValue(frame._submissionIndex, UINT64_MAX);
		}

		ReleaseCompletedDeletions(_submissionEvent->signaledValue());

		frame._commandAllocator->reset();
		frame._transientBufferIndex = 0;
		frame._transientBufferOffset = 0;
	}
}
