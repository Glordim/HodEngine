#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/Allocator.hpp"

namespace hod
{
	class Allocator;

	class HOD_CORE_API DefaultAllocator
	{
	public:
		DefaultAllocator();

		static Allocator& GetInstance();

	private:
		static DefaultAllocator _defaultAllocator;
	};
}
