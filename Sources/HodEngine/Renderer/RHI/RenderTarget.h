#pragma once

#include <cstdint>
#include <cstddef>

namespace hod
{
	namespace renderer
	{
		class Texture;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderTarget
		{
		public:

							RenderTarget();
			virtual			~RenderTarget();

			uint32_t		GetWidth() const;
			uint32_t		GetHeight() const;

			virtual bool	Init(size_t width, size_t height);

			Texture*		GetColorTexture() const;
			Texture*		GetDepthTexture() const;

			virtual void	PrepareForWrite() = 0;
			virtual void	PrepareForRead() = 0;

		protected:

			virtual void	Clear();

		protected:

			Texture*		_color = nullptr;
			Texture*		_depth = nullptr;

		protected:

			uint32_t		_width = 0;
			uint32_t		_height = 0;
		};
	}
}
