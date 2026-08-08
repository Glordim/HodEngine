#include "HodEngine/Editor/Pch.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#define STBI_MALLOC(sz)             hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(sz))
#define STBI_REALLOC(p, newsz)      hod::DefaultAllocator::GetInstance().Reallocate(p, static_cast<uint32_t>(newsz))
#define STBI_FREE(p)                hod::DefaultAllocator::GetInstance().Free(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STBIW_MALLOC(sz)            hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(sz))
#define STBIW_REALLOC(p, newsz)     hod::DefaultAllocator::GetInstance().Reallocate(p, static_cast<uint32_t>(newsz))
#define STBIW_FREE(p)               hod::DefaultAllocator::GetInstance().Free(p)

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STBIR_MALLOC(size, user_data) ((void)(user_data), hod::DefaultAllocator::GetInstance().Allocate(static_cast<uint32_t>(size)))
#define STBIR_FREE(ptr, user_data)    ((void)(user_data), hod::DefaultAllocator::GetInstance().Free(ptr))

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>
