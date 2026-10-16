#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"
#include "HodEngine/RHI/D3d12/D3d12DescriptorHeap.hpp"
#include "HodEngine/RHI/D3d12/D3d12GpuDevice.hpp"

namespace hod::inline rhi
{
	class D3d12Sampler;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API D3d12RhiDevice : public RhiDevice
	{
		_SingletonOverride(D3d12RhiDevice)

	protected:
		~D3d12RhiDevice() override;

	public:
		void WaitIdle() override;

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		PresentationSurface* CreatePresentationSurface(window::Window* window) override;

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr, const Semaphore* waitSemaphore = nullptr,
		                          const Fence* fence = nullptr) override;

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

		ID3D12Device*       GetDevice() const;
		IDXGIFactory6*      GetDxgiFactory() const;
		ID3D12CommandQueue* GetCommandQueue() const;

		D3d12DescriptorHeap& GetRenderTargetViewHeap();
		D3d12DescriptorHeap& GetDepthStencilViewHeap();
		D3d12DescriptorHeap& GetShaderResourceHeap(); // shader visible
		D3d12DescriptorHeap& GetSamplerHeap();        // shader visible

		// Not every driver draws triangle fans, see D3d12CommandBuffer::Draw
		bool IsTriangleFanSupported() const;

		// Command buffers are created and deleted every frame, their D3D12 objects are recycled
		bool AcquireCommandList(ComPtr<ID3D12CommandAllocator>& commandAllocator, ComPtr<ID3D12GraphicsCommandList>& commandList);
		void ReleaseCommandList(ComPtr<ID3D12CommandAllocator>& commandAllocator, ComPtr<ID3D12GraphicsCommandList>& commandList);

		// Commands that upload data into resources, recorded at any time without the CPU waiting for them: they are executed ahead of
		// the next SubmitCommandBuffers, so that what they upload is ready for it. What is released meanwhile is kept until they are done.
		ID3D12GraphicsCommandList* GetUploadCommandList();

		// Table of the sampler heap holding the given samplers, in that order.
		// The sampler heap is small (2048 descriptors) while BindGroups are many: they share the table of their samplers.
		bool GetSamplerTable(const D3d12Sampler* const* samplers, uint32_t samplerCount, D3D12_GPU_DESCRIPTOR_HANDLE& samplerTable);

		// Released, or freed, once the frames in flight are done with it
		void DeferRelease(ComPtr<IUnknown> object);
		void DeferFree(D3d12DescriptorHeap& descriptorHeap, uint32_t index, uint32_t count = 1);

	protected:
		bool     InitDevice(uint32_t physicalDeviceIdentifier) override;
		Sampler* CreateSampler(const Sampler::CreateInfo& createInfo) override;
		void     FlushDeferredDeletions(uint32_t frameIndex) override;

	private:
		void EnumerateGpuDevices();
		bool SelectGpuDevice(uint32_t physicalDeviceIdentifier);
		bool CreateDevice();
		void SetupValidationOutput();

		// Closes the pending upload commands, for the caller to execute them now. Null when there are none.
		ID3D12CommandList* EndUploads();
		void               FlushUploads(); // executes them on their own, for WaitIdle

		// To call after each submission: gives it the next submission index
		bool SignalSubmissionIndex();
		// What is released now may be used by the submissions up to the next one, which carries the pending upload commands
		uint64_t GetDeferredDeletionSubmissionIndex() const;
		void     ReleaseCompletedDeletions(uint64_t completedSubmissionIndex);
		void OutputStoredValidationMessages();

	private:
		struct CommandList
		{
			ComPtr<ID3D12CommandAllocator>    _commandAllocator;
			ComPtr<ID3D12GraphicsCommandList> _commandList;
		};

		struct SamplerTable
		{
			Vector<Sampler::CreateInfo> _samplers;
			uint32_t                    _index = D3d12DescriptorHeap::InvalidIndex;
		};

		struct DeferredDescriptors
		{
			D3d12DescriptorHeap* _descriptorHeap = nullptr;
			uint32_t             _index = 0;
			uint32_t             _count = 0;
			uint64_t             _submissionIndex = 0;
		};

		// Released with the index of the next submission: released once the GPU is done with it, the last one that may use them
		struct DeferredObject
		{
			IUnknown* _object = nullptr; // holds a reference (a ComPtr can't be stored in a Vector, which takes the address of its elements)
			uint64_t  _submissionIndex = 0;
		};

		struct DeferredCommandList
		{
			CommandList _commandList;
			uint64_t    _submissionIndex = 0;
		};

	private:
		Vector<D3d12GpuDevice> _availableGpu;
		const D3d12GpuDevice*  _selectedGpu = nullptr;
		const D3d12GpuDevice*  _recommandedGpu = nullptr;

		ComPtr<IDXGIFactory6>      _dxgiFactory;
		ComPtr<ID3D12Device>       _device;
		ComPtr<ID3D12CommandQueue> _commandQueue;

		bool                    _validationEnabled = false;
		ComPtr<ID3D12InfoQueue> _infoQueue;                    // only with the validation layer
		DWORD                   _validationCallbackCookie = 0; // 0 when the messages have to be polled instead

		// Every submission to the queue gets the next index, which it signals once the GPU is done with it
		ComPtr<ID3D12Fence> _submissionFence;
		uint64_t            _lastSubmissionIndex = 0;
		HANDLE              _idleEvent = nullptr;

		D3d12DescriptorHeap _renderTargetViewHeap;
		D3d12DescriptorHeap _depthStencilViewHeap;
		D3d12DescriptorHeap _shaderResourceHeap;
		D3d12DescriptorHeap _samplerHeap;

		Vector<SamplerTable> _samplerTables;

		Vector<CommandList> _freeCommandLists;
		CommandList         _uploadCommandList; // recording while its list is not null, until EndUploads

		bool _triangleFanSupported = false;

		Vector<DeferredObject>      _objectsToRelease;
		Vector<DeferredDescriptors> _descriptorsToFree;
		Vector<DeferredCommandList> _commandListsToRelease; // back to _freeCommandLists
		bool                        _deferredDeletionsClosed = false; // from the destruction on, what is released goes right away
	};
}
