#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Allocator : public physx::PxAllocatorCallback
		{
		public:
					Allocator();
					~Allocator() override;

		public:

			void*	allocate(size_t size, const char* typeName, const char* filename, int line) override;
			void	deallocate(void* ptr) override;
		};
	}
}
