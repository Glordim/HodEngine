#ifndef __ALLOCATOR_HPP__
#define __ALLOCATOR_HPP__

#include <PxPhysicsAPI.h>

class Allocator : public physx::PxAllocatorCallback
{
public:
    Allocator();
    virtual ~Allocator();

    virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
    virtual void deallocate(void* ptr) override;
};

#endif
