#include "Allocator.h"

namespace HOD
{
    Allocator::Allocator()
    {
    }

    Allocator::~Allocator()
    {
    }

    void* Allocator::allocate(size_t size, const char* typeName, const char* filename, int line)
    {
        return new char[size];
    }

    void Allocator::deallocate(void* ptr)
    {
        delete[] ptr;
    }
}
