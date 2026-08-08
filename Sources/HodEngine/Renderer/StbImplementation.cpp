#include "HodEngine/Renderer/Pch.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#define STBI_MALLOC(sz)             hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(sz))
#define STBI_REALLOC(p, newsz)      hod::DefaultAllocator::GetInstance().Reallocate(p, static_cast<uint32_t>(newsz))
#define STBI_FREE(p)                hod::DefaultAllocator::GetInstance().Free(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
