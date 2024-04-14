#pragma once

#include <cstdint>
#include <cstddef>

#include <HodEngine/Core/Color.hpp>
#include "HodEngine/Renderer/Enums.hpp"

namespace hod
{
	class Vector2;

	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Texture
		{
		public:

			struct CreateInfo
			{
				WrapMode	_wrapMode = WrapMode::Clamp;
				FilterMode	_filterMode = FilterMode::Linear;
				bool		_allowReadWrite = false;
			};

		public:

			Texture();
			virtual ~Texture();

			bool LoadFromPath(const char* path);

			uint32_t	GetWidth() const;
			uint32_t	GetHeight() const;

			virtual bool BuildColor(size_t width, size_t height, const CreateInfo& createInfo) = 0;
			virtual bool BuildDepth(size_t width, size_t height, const CreateInfo& createInfo) = 0;
			virtual bool BuildBuffer(size_t width, size_t height, const uint8_t* buffer, const CreateInfo& createInfo) = 0;

			virtual Color ReadPixel(const Vector2& position) const = 0;

		protected:

			uint32_t	_width = 0;
			uint32_t	_height = 0;
		};
	}
}
