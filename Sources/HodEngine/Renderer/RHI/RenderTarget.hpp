#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <cstdint>
#include <cstddef>

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API RenderTarget
	{
	public:

						RenderTarget();
		virtual			~RenderTarget();

		uint32_t		GetWidth() const;
		uint32_t		GetHeight() const;

		virtual bool	Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo);

		Texture*		GetColorTexture() const;
		Texture*		GetDepthTexture() const;

		virtual void	PrepareForWrite() = 0;
		virtual void	PrepareForRead() = 0;

		bool			IsValid() const;

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
