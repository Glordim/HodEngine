#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#define REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER
/*                                                                                                                            \
void* operator new(std::size_t count) { return hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(count)); } \
void* operator new(std::size_t count, std::align_val_t alignment) { return hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(count), static_cast<uint32_t>(alignment)); } \
void operator delete(void* ptr) noexcept { hod::DefaultAllocator::GetInstance().Free(ptr); } \
void operator delete(void* ptr, std::align_val_t alignment) noexcept { hod::DefaultAllocator::GetInstance().Free(ptr); }
*/
