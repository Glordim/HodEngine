#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/UniformAllocator.hpp"

#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"

#include <HodEngine/Core/Assert.hpp>

namespace hod::inline renderer
{
	/// @brief
	UniformAllocator::~UniformAllocator()
	{
		Assert(_pages.Empty()); // Clear must be called while the RhiDevice is still there
	}

	/// @brief
	/// @param size
	/// @return an Allocation with a null buffer if no buffer can be created
	UniformAllocator::Allocation UniformAllocator::Allocate(uint32_t size)
	{
		uint32_t alignment = RhiDevice::GetInstance()->GetUniformBufferOffsetAlignment();
		uint32_t alignedSize = (size + alignment - 1) / alignment * alignment;

		while (_currentPage < _pages.Size() && _currentOffset + alignedSize > _pages[_currentPage]._size)
		{
			++_currentPage;
			_currentOffset = 0;
		}

		if (_currentPage == _pages.Size() && AddPage(alignedSize) == false)
		{
			return Allocation();
		}

		const Page& page = _pages[_currentPage];

		Allocation allocation;
		allocation._buffer = page._buffer;
		allocation._offset = _currentOffset;
		allocation._data = page._data + _currentOffset;

		_currentOffset += alignedSize;
		return allocation;
	}

	/// @brief
	/// @param minimumSize
	/// @return
	bool UniformAllocator::AddPage(uint32_t minimumSize)
	{
		Page page;
		page._size = minimumSize > PageSize ? minimumSize : PageSize;
		page._buffer = RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Uniform, page._size);
		if (page._buffer == nullptr)
		{
			return false;
		}

		page._data = static_cast<uint8_t*>(page._buffer->Lock());
		if (page._data == nullptr)
		{
			DefaultAllocator::GetInstance().Delete(page._buffer);
			return false;
		}

		_pages.PushBack(page);
		return true;
	}

	/// @brief
	void UniformAllocator::Reset()
	{
		_currentPage = 0;
		_currentOffset = 0;
	}

	/// @brief
	void UniformAllocator::Clear()
	{
		for (const Page& page : _pages)
		{
			page._buffer->Unlock();
			DefaultAllocator::GetInstance().Delete(page._buffer);
		}
		_pages.Clear();

		Reset();
	}
}
