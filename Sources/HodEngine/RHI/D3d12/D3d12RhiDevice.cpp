#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"

#include "HodEngine/RHI/D3d12/D3d12BindGroup.hpp"
#include "HodEngine/RHI/D3d12/D3d12Buffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12CommandBuffer.hpp"
#include "HodEngine/RHI/D3d12/D3d12Fence.hpp"
#include "HodEngine/RHI/D3d12/D3d12GraphicsPipeline.hpp"
#include "HodEngine/RHI/D3d12/D3d12PresentationSurface.hpp"
#include "HodEngine/RHI/D3d12/D3d12Sampler.hpp"
#include "HodEngine/RHI/D3d12/D3d12Semaphore.hpp"
#include "HodEngine/RHI/D3d12/D3d12Shader.hpp"
#include "HodEngine/RHI/D3d12/D3d12Texture.hpp"

#include "HodEngine/RHI/RenderTarget.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Window.hpp>

namespace hod::inline rhi
{
	namespace
	{
		constexpr uint32_t RenderTargetViewHeapCapacity = 1024;
		constexpr uint32_t ShaderResourceHeapCapacity = 65536;
		constexpr uint32_t SamplerHeapCapacity = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		void OutputValidationMessage(D3D12_MESSAGE_SEVERITY severity, const char* description)
		{
			switch (severity)
			{
				case D3D12_MESSAGE_SEVERITY_CORRUPTION:
				case D3D12_MESSAGE_SEVERITY_ERROR: OUTPUT_ERROR("D3d12: {}", description); break;
				case D3D12_MESSAGE_SEVERITY_WARNING: OUTPUT_WARNING("D3d12: {}", description); break;
				default: OUTPUT_MESSAGE("D3d12: {}", description); break;
			}
		}

		void CALLBACK ValidationMessageCallback(D3D12_MESSAGE_CATEGORY /*category*/, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID /*id*/, LPCSTR description,
		                                        void* /*context*/)
		{
			OutputValidationMessage(severity, description);
		}
#endif
	}

	_SingletonOverrideConstructor(D3d12RhiDevice)
	: RhiDevice()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12RhiDevice::~D3d12RhiDevice()
	{
		WaitIdle();

		for (uint32_t frameIndex = 0; frameIndex < _objectsToRelease.Size(); ++frameIndex)
		{
			FlushDeferredDeletions(frameIndex);
		}
		// From now on DeferRelease and DeferFree act immediately
		_objectsToRelease.Clear();
		_descriptorsToFree.Clear();

		_freeCommandLists.Clear();
		_immediateCommandList = CommandList();

		_samplerTables.Clear();
		_samplerHeap.Release();
		_shaderResourceHeap.Release();
		_renderTargetViewHeap.Release();

		if (_idleEvent != nullptr)
		{
			CloseHandle(_idleEvent);
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		if (_infoQueue != nullptr)
		{
			OutputStoredValidationMessages();

			ComPtr<ID3D12InfoQueue1> infoQueue1;
			if (_validationCallbackCookie != 0 && SUCCEEDED(_infoQueue.As(&infoQueue1)))
			{
				infoQueue1->UnregisterMessageCallback(_validationCallbackCookie);
			}
		}
#endif
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12RhiDevice::InitDevice(uint32_t physicalDeviceIdentifier)
	{
		_objectsToRelease.Resize(GetFrameInFlightCount());
		_descriptorsToFree.Resize(GetFrameInFlightCount());

		UINT factoryFlags = 0;

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		ComPtr<ID3D12Debug> debugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			_validationEnabled = true;
		}
		else
		{
			OUTPUT_WARNING("D3d12: Validation layer requested, but not available (it comes with the \"Graphics Tools\" optional feature of Windows)");
		}
#endif

		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&_dxgiFactory))))
		{
			OUTPUT_ERROR("D3d12: Unable to create the DXGI factory!");
			return false;
		}

		EnumerateGpuDevices();

		if (SelectGpuDevice(physicalDeviceIdentifier) == false)
		{
			return false;
		}

		if (CreateDevice() == false)
		{
			return false;
		}

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		if (FAILED(_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&_commandQueue))))
		{
			OUTPUT_ERROR("D3d12: Unable to create the command queue!");
			return false;
		}

		if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_idleFence))))
		{
			OUTPUT_ERROR("D3d12: Unable to create fence!");
			return false;
		}

		_idleEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (_idleEvent == nullptr)
		{
			OUTPUT_ERROR("D3d12: Unable to create fence event!");
			return false;
		}

		if (_renderTargetViewHeap.Init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RenderTargetViewHeapCapacity, false) == false ||
		    _shaderResourceHeap.Init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, ShaderResourceHeapCapacity, true) == false ||
		    _samplerHeap.Init(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SamplerHeapCapacity, true) == false)
		{
			return false;
		}

		if (AcquireCommandList(_immediateCommandList._commandAllocator, _immediateCommandList._commandList) == false)
		{
			return false;
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS15 options15 = {};
		if (SUCCEEDED(_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS15, &options15, sizeof(options15))))
		{
			_triangleFanSupported = options15.TriangleFanSupported == TRUE;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12RhiDevice::EnumerateGpuDevices()
	{
		_availableGpu.Clear();
		_recommandedGpu = nullptr;

		// From the most to the least powerful, according to the system
		ComPtr<IDXGIAdapter1> adapter;
		for (UINT adapterIndex = 0;
		     SUCCEEDED(_dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
		     ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 adapterDesc;
			if (FAILED(adapter->GetDesc1(&adapterDesc)))
			{
				continue;
			}

			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue; // not the Basic Render Driver
			}

			D3d12GpuDevice gpuDevice;
			gpuDevice.adapter = adapter;
			gpuDevice.deviceId = adapterDesc.DeviceId;
			gpuDevice.name = adapterDesc.Description;
			gpuDevice.vram = adapterDesc.DedicatedVideoMemory / 1000000;
			gpuDevice.score = gpuDevice.vram;
			gpuDevice.compatible = SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr));

			_availableGpu.PushBack(gpuDevice);
		}

		for (const D3d12GpuDevice& gpuDevice : _availableGpu)
		{
			if (gpuDevice.compatible)
			{
				_recommandedGpu = &gpuDevice;
				break;
			}
		}
	}

	/// @brief
	/// @param physicalDeviceIdentifier
	/// @return
	bool D3d12RhiDevice::SelectGpuDevice(uint32_t physicalDeviceIdentifier)
	{
		_selectedGpu = nullptr;

		if (physicalDeviceIdentifier != 0)
		{
			for (const D3d12GpuDevice& gpuDevice : _availableGpu)
			{
				if (gpuDevice.deviceId == physicalDeviceIdentifier)
				{
					_selectedGpu = &gpuDevice;
					break;
				}
			}
			if (_selectedGpu == nullptr)
			{
				OUTPUT_ERROR("D3d12: Adapter {} not found, fallback on recommanded adapter", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
			else if (_selectedGpu->compatible == false)
			{
				OUTPUT_ERROR("D3d12: Selected adapter {} is not compatible with GameEngine requirement, fallback on recommanded adapter", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
		}
		else
		{
			_selectedGpu = _recommandedGpu;
		}

		if (_selectedGpu == nullptr)
		{
			OUTPUT_ERROR("D3d12: No adapter compatible with GameEngine requirement... sorry");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12RhiDevice::CreateDevice()
	{
		if (FAILED(D3D12CreateDevice(_selectedGpu->adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
		{
			OUTPUT_ERROR("D3d12: Unable to create device!");
			return false;
		}

		SetupValidationOutput();

		return true;
	}

	/// @brief Sends what the validation layer reports to the OutputService
	void D3d12RhiDevice::SetupValidationOutput()
	{
		if (_validationEnabled == false)
		{
			return;
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		if (FAILED(_device.As(&_infoQueue)))
		{
			return;
		}

		D3D12_MESSAGE_SEVERITY deniedSeverities[] = {D3D12_MESSAGE_SEVERITY_INFO};

		D3D12_MESSAGE_ID deniedIds[] = {
			// Render targets are cleared with the color of each render pass, which is not known when they are created
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumSeverities = _countof(deniedSeverities);
		filter.DenyList.pSeverityList = deniedSeverities;
		filter.DenyList.NumIDs = _countof(deniedIds);
		filter.DenyList.pIDList = deniedIds;
		_infoQueue->PushStorageFilter(&filter);

		// Without ID3D12InfoQueue1 (before Windows 11) the messages are stored, and output once per frame
		ComPtr<ID3D12InfoQueue1> infoQueue1;
		if (SUCCEEDED(_infoQueue.As(&infoQueue1)))
		{
			if (FAILED(infoQueue1->RegisterMessageCallback(ValidationMessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &_validationCallbackCookie)))
			{
				_validationCallbackCookie = 0;
			}
		}
#endif
	}

	/// @brief
	void D3d12RhiDevice::OutputStoredValidationMessages()
	{
		if (_infoQueue == nullptr || _validationCallbackCookie != 0)
		{
			return;
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		Vector<uint8_t> messageBuffer;

		UINT64 messageCount = _infoQueue->GetNumStoredMessages();
		for (UINT64 messageIndex = 0; messageIndex < messageCount; ++messageIndex)
		{
			SIZE_T messageSize = 0;
			if (FAILED(_infoQueue->GetMessage(messageIndex, nullptr, &messageSize)) || messageSize == 0)
			{
				continue;
			}

			messageBuffer.Resize((uint32_t)messageSize);
			D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(messageBuffer.Data());
			if (SUCCEEDED(_infoQueue->GetMessage(messageIndex, message, &messageSize)))
			{
				OutputValidationMessage(message->Severity, message->pDescription);
			}
		}
		_infoQueue->ClearStoredMessages();
#endif
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void D3d12RhiDevice::WaitIdle()
	{
		if (_commandQueue == nullptr || _idleFence == nullptr)
		{
			return;
		}

		++_idleFenceValue;
		if (FAILED(_commandQueue->Signal(_idleFence.Get(), _idleFenceValue)))
		{
			OUTPUT_ERROR("D3d12: WaitIdle failed!");
			return;
		}

		if (_idleFence->GetCompletedValue() < _idleFenceValue)
		{
			if (FAILED(_idleFence->SetEventOnCompletion(_idleFenceValue, _idleEvent)))
			{
				OUTPUT_ERROR("D3d12: WaitIdle failed!");
				return;
			}
			WaitForSingleObject(_idleEvent, INFINITE);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12RhiDevice::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		if (availableDevices == nullptr)
		{
			return false;
		}

		availableDevices->Resize(_availableGpu.Size());
		for (uint32_t index = 0; index < _availableGpu.Size(); ++index)
		{
			availableDevices->At(index) = &_availableGpu[index];
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12Device* D3d12RhiDevice::GetDevice() const
	{
		return _device.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	IDXGIFactory6* D3d12RhiDevice::GetDxgiFactory() const
	{
		return _dxgiFactory.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12CommandQueue* D3d12RhiDevice::GetCommandQueue() const
	{
		return _commandQueue.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12DescriptorHeap& D3d12RhiDevice::GetRenderTargetViewHeap()
	{
		return _renderTargetViewHeap;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12DescriptorHeap& D3d12RhiDevice::GetShaderResourceHeap()
	{
		return _shaderResourceHeap;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	D3d12DescriptorHeap& D3d12RhiDevice::GetSamplerHeap()
	{
		return _samplerHeap;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12RhiDevice::IsTriangleFanSupported() const
	{
		return _triangleFanSupported;
	}

	/// @brief
	/// @param commandAllocator
	/// @param commandList closed: it has to be reset before recording
	/// @return
	bool D3d12RhiDevice::AcquireCommandList(ComPtr<ID3D12CommandAllocator>& commandAllocator, ComPtr<ID3D12GraphicsCommandList>& commandList)
	{
		if (_freeCommandLists.Empty() == false)
		{
			commandAllocator = std::move(_freeCommandLists.Back()._commandAllocator);
			commandList = std::move(_freeCommandLists.Back()._commandList);
			_freeCommandLists.PopBack();
			return true;
		}

		if (FAILED(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
		{
			OUTPUT_ERROR("D3d12: Unable to create command allocator!");
			return false;
		}

		if (FAILED(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
		{
			OUTPUT_ERROR("D3d12: Unable to create command list!");
			commandAllocator.Reset();
			return false;
		}
		commandList->Close(); // created recording

		return true;
	}

	/// @brief The GPU must be done with what the command list recorded
	/// @param commandAllocator
	/// @param commandList
	void D3d12RhiDevice::ReleaseCommandList(ComPtr<ID3D12CommandAllocator>& commandAllocator, ComPtr<ID3D12GraphicsCommandList>& commandList)
	{
		if (commandAllocator == nullptr || commandList == nullptr)
		{
			return;
		}

		CommandList freeCommandList;
		freeCommandList._commandAllocator = std::move(commandAllocator);
		freeCommandList._commandList = std::move(commandList);
		_freeCommandLists.PushBack(std::move(freeCommandList));
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	ID3D12GraphicsCommandList* D3d12RhiDevice::BeginImmediateCommands()
	{
		if (FAILED(_immediateCommandList._commandAllocator->Reset()) || FAILED(_immediateCommandList._commandList->Reset(_immediateCommandList._commandAllocator.Get(), nullptr)))
		{
			OUTPUT_ERROR("D3d12: Unable to begin immediate commands!");
			return nullptr;
		}
		return _immediateCommandList._commandList.Get();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool D3d12RhiDevice::EndImmediateCommands()
	{
		if (FAILED(_immediateCommandList._commandList->Close()))
		{
			OUTPUT_ERROR("D3d12: Unable to end immediate commands!");
			return false;
		}

		ID3D12CommandList* commandLists[] = {_immediateCommandList._commandList.Get()};
		_commandQueue->ExecuteCommandLists(1, commandLists);

		WaitIdle();
		return true;
	}

	/// @brief
	/// @param samplers
	/// @param samplerCount
	/// @param samplerTable
	/// @return
	bool D3d12RhiDevice::GetSamplerTable(const D3d12Sampler* const* samplers, uint32_t samplerCount, D3D12_GPU_DESCRIPTOR_HANDLE& samplerTable)
	{
		// Samplers are compared by their settings: unlike their address, it tells them apart for as long as the table lives
		for (const SamplerTable& existingTable : _samplerTables)
		{
			if (existingTable._samplers.Size() != samplerCount)
			{
				continue;
			}

			bool same = true;
			for (uint32_t index = 0; index < samplerCount && same; ++index)
			{
				same = existingTable._samplers[index] == samplers[index]->GetCreateInfo();
			}
			if (same)
			{
				samplerTable = _samplerHeap.GetGpuHandle(existingTable._index);
				return true;
			}
		}

		SamplerTable newTable;
		newTable._index = _samplerHeap.Allocate(samplerCount);
		if (newTable._index == D3d12DescriptorHeap::InvalidIndex)
		{
			OUTPUT_ERROR("D3d12: The sampler heap is full!");
			return false;
		}

		for (uint32_t index = 0; index < samplerCount; ++index)
		{
			_device->CreateSampler(&samplers[index]->GetSamplerDesc(), _samplerHeap.GetCpuHandle(newTable._index + index));
			newTable._samplers.PushBack(samplers[index]->GetCreateInfo());
		}

		samplerTable = _samplerHeap.GetGpuHandle(newTable._index);
		_samplerTables.PushBack(std::move(newTable));
		return true;
	}

	/// @brief
	/// @param object
	void D3d12RhiDevice::DeferRelease(ComPtr<IUnknown> object)
	{
		if (object != nullptr && _objectsToRelease.Empty() == false)
		{
			_objectsToRelease[_frameIndex].PushBack(object.Detach());
		}
	}

	/// @brief
	/// @param descriptorHeap
	/// @param index
	/// @param count
	void D3d12RhiDevice::DeferFree(D3d12DescriptorHeap& descriptorHeap, uint32_t index, uint32_t count)
	{
		if (_descriptorsToFree.Empty())
		{
			descriptorHeap.Free(index, count);
			return;
		}

		DeferredDescriptors deferredDescriptors;
		deferredDescriptors._descriptorHeap = &descriptorHeap;
		deferredDescriptors._index = index;
		deferredDescriptors._count = count;
		_descriptorsToFree[_frameIndex].PushBack(deferredDescriptors);
	}

	/// @brief
	/// @param frameIndex
	void D3d12RhiDevice::FlushDeferredDeletions(uint32_t frameIndex)
	{
		for (IUnknown* object : _objectsToRelease[frameIndex])
		{
			object->Release();
		}
		_objectsToRelease[frameIndex].Clear();

		for (const DeferredDescriptors& deferredDescriptors : _descriptorsToFree[frameIndex])
		{
			deferredDescriptors._descriptorHeap->Free(deferredDescriptors._index, deferredDescriptors._count);
		}
		_descriptorsToFree[frameIndex].Clear();

		OutputStoredValidationMessages();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	PresentationSurface* D3d12RhiDevice::CreatePresentationSurface(window::Window* window)
	{
		D3d12PresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<D3d12PresentationSurface>(window);
		presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		_presentationSurfaces.PushBack(presentationSurface);
		return presentationSurface;
	}

	/// @brief
	/// @param commandBuffers
	/// @param commandBufferCount
	/// @param signalSemaphore
	/// @param waitSemaphore
	/// @param fence
	/// @return
	bool D3d12RhiDevice::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* /*signalSemaphore*/, const Semaphore* /*waitSemaphore*/,
	                                          const Fence* fence)
	{
		// Everything goes through the one command queue, which executes in the order it is given the work, presents included:
		// there is nothing left for the semaphores to order

		Vector<ID3D12CommandList*> commandLists(commandBufferCount);
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			commandLists[commandBufferIndex] = static_cast<D3d12CommandBuffer*>(commandBuffers[commandBufferIndex])->GetCommandList();
		}

		if (commandBufferCount > 0)
		{
			_commandQueue->ExecuteCommandLists(commandBufferCount, commandLists.Data());
		}

		if (fence != nullptr && static_cast<const D3d12Fence*>(fence)->Signal(_commandQueue.Get()) == false)
		{
			OUTPUT_ERROR("D3d12: Unable to submit draw command buffer!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Shader* D3d12RhiDevice::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<D3d12Shader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline* D3d12RhiDevice::CreateGraphicsPipeline(const GraphicsPipeline::CreateInfo& createInfo)
	{
		D3d12GraphicsPipeline* graphicsPipeline = DefaultAllocator::GetInstance().New<D3d12GraphicsPipeline>();

		if (graphicsPipeline->Build(createInfo) == false)
		{
			DefaultAllocator::GetInstance().Delete(graphicsPipeline);
			return nullptr;
		}

		return graphicsPipeline;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	CommandBuffer* D3d12RhiDevice::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<D3d12CommandBuffer>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Buffer* D3d12RhiDevice::CreateBuffer(Buffer::Usage usage, uint32_t size)
	{
		return DefaultAllocator::GetInstance().New<D3d12Buffer>(usage, size);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Texture* D3d12RhiDevice::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<D3d12Texture>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	RenderTarget* D3d12RhiDevice::CreateRenderTarget()
	{
		// Nothing more than its textures, which hold what it takes to draw into them
		return DefaultAllocator::GetInstance().New<RenderTarget>();
	}

	/// @brief
	/// @return
	Semaphore* D3d12RhiDevice::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<D3d12Semaphore>();
	}

	/// @brief
	/// @return
	Fence* D3d12RhiDevice::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<D3d12Fence>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	BindGroup* D3d12RhiDevice::CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                                           const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		if (graphicsPipeline == nullptr)
		{
			return nullptr;
		}

		D3d12BindGroup* bindGroup = DefaultAllocator::GetInstance().New<D3d12BindGroup>();
		if (bindGroup->Build(*static_cast<const D3d12GraphicsPipeline*>(graphicsPipeline), set, uniformBuffers, uniformBufferCount, textureBindings, textureBindingCount) == false)
		{
			DefaultAllocator::GetInstance().Delete(bindGroup);
			return nullptr;
		}
		return bindGroup;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Sampler* D3d12RhiDevice::CreateSampler(const Sampler::CreateInfo& createInfo)
	{
		return DefaultAllocator::GetInstance().New<D3d12Sampler>(createInfo);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	uint32_t D3d12RhiDevice::GetUniformBufferOffsetAlignment() const
	{
		return D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	}
}
