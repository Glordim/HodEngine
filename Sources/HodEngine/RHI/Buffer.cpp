#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Buffer.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Buffer::Buffer(Usage usage, uint32_t Size)
	: _usage(usage)
	, _size(Size)
	{
	}
}
