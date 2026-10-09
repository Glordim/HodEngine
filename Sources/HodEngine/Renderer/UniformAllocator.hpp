#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline rhi
{
	class Buffer;
}

namespace hod::inline renderer
{
	/// @brief Uniform data of one frame, written linearly into a few large buffers.
	/// Each frame in flight has its own allocator (see FrameResources): what a frame wrote stays untouched until that frame is done,
	/// which is what lets a MaterialInstance change its values while a previous frame still reads the old ones.
	class HOD_RENDERER_API UniformAllocator
	{
	public:
		struct Allocation
		{
			Buffer*  _buffer = nullptr;
			uint32_t _offset = 0; // multiple of RhiDevice::GetUniformBufferOffsetAlignment
			void*    _data = nullptr;
		};

	public:
		UniformAllocator() = default;
		UniformAllocator(const UniformAllocator&) = delete;
		UniformAllocator(UniformAllocator&&) = default;
		~UniformAllocator();

		UniformAllocator& operator=(const UniformAllocator&) = delete;
		UniformAllocator& operator=(UniformAllocator&&) = default;

	public:
		Allocation Allocate(uint32_t size);

		// Makes everything allocated so far reusable; the buffers are kept
		void Reset();

		// Releases the buffers
		void Clear();

	private:
		// A buffer stays mapped for its whole life
		struct Page
		{
			Buffer*  _buffer = nullptr;
			uint8_t* _data = nullptr;
			uint32_t _size = 0;
		};

		bool AddPage(uint32_t minimumSize);

	private:
		static constexpr uint32_t PageSize = 64 * 1024;

		Vector<Page> _pages;
		uint32_t     _currentPage = 0;
		uint32_t     _currentOffset = 0;
	};
}
