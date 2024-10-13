#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/MemoryManager.hpp"

#define REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER \
void* operator new(std::size_t count) { return hod::MemoryManager::GetInstance()->Allocate(static_cast<uint32_t>(count)); } \
void* operator new(std::size_t count, std::align_val_t alignment) { return hod::MemoryManager::GetInstance()->AllocateAlign(static_cast<uint32_t>(count), static_cast<uint32_t>(alignment)); } \
void operator delete(void* ptr) noexcept { hod::MemoryManager::GetInstance()->Free(ptr); } \
void operator delete(void* ptr, std::align_val_t alignment) noexcept { hod::MemoryManager::GetInstance()->FreeAlign(ptr, static_cast<uint32_t>(alignment)); }
