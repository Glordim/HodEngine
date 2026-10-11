#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12Fence.hpp"
#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline rhi
{
	/// @brief
	D3d12Fence::D3d12Fence()
	: Fence()
	{
		if (FAILED(D3d12RhiDevice::GetInstance()->GetDevice()->CreateFence(_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
		{
			OUTPUT_ERROR("D3d12: Unable to create fence!");
			return;
		}

		_event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (_event == nullptr)
		{
			OUTPUT_ERROR("D3d12: Unable to create fence event!");
		}
	}

	/// @brief
	D3d12Fence::~D3d12Fence()
	{
		if (_event != nullptr)
		{
			CloseHandle(_event);
		}
	}

	/// @brief A D3D12 fence only counts up: there is nothing to reset, the next Signal sets what Wait waits for
	/// @return
	bool D3d12Fence::Reset()
	{
		return _fence != nullptr;
	}

	/// @brief
	/// @return
	bool D3d12Fence::Wait()
	{
		if (_fence == nullptr || _event == nullptr)
		{
			return false;
		}

		if (_fence->GetCompletedValue() < _value)
		{
			if (FAILED(_fence->SetEventOnCompletion(_value, _event)))
			{
				OUTPUT_ERROR("D3d12: Unable to wait fence");
				return false;
			}
			WaitForSingleObject(_event, INFINITE);
		}
		return true;
	}

	/// @brief
	/// @param commandQueue
	/// @return
	bool D3d12Fence::Signal(ID3D12CommandQueue* commandQueue) const
	{
		if (_fence == nullptr)
		{
			return false;
		}

		if (FAILED(commandQueue->Signal(_fence.Get(), _value + 1)))
		{
			return false;
		}
		++_value;
		return true;
	}
}
