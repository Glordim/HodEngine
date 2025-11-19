#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Buffer.hpp"

namespace hod
{
	namespace renderer
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
}
