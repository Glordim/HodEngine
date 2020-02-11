#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
	namespace PHYSIC
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Allocator : public physx::PxAllocatorCallback
		{
		public:
							Allocator();
							~Allocator() override;

			void*	allocate(size_t size, const char* typeName, const char* filename, int line) override;
			void	deallocate(void* ptr) override;
		};
	}
}
