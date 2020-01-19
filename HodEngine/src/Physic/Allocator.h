#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
    class Allocator : public physx::PxAllocatorCallback
    {
    public:
        Allocator();
        virtual ~Allocator();

        virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
        virtual void deallocate(void* ptr) override;
    };
}
