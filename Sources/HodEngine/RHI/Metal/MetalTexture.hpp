#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Texture.hpp"

namespace MTL
{
	class Texture;
	class TextureDescriptor;
}

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RHI_API MetalTexture : public Texture
	{
	public:

		// MTL::PixelFormatInvalid (0) for PixelFormat::None. R8G8B8A8_UNorm is BGRA8Unorm, the format of the presentation surfaces
		static uint32_t	ToMetalPixelFormat(PixelFormat format);

	public:

						MetalTexture();
						~MetalTexture() override;

		bool			BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool			BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool			BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) override;

		Color			ReadPixel(const Vector2& position) const override;
		void			CaptureReadback(CommandBuffer* commandBuffer) override;

		MTL::Texture*       GetNativeTexture() const;

	protected:

		bool			UploadRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels, uint32_t mipLevel) override;

	private:

		bool			Build(MTL::TextureDescriptor* textureDescriptor, uint32_t width, uint32_t height, const CreateInfo& createInfo);
		void			Release();

	private:

		MTL::Texture*       _texture = nullptr;

		bool				_readable = false; // built with CreateInfo::_allowReadWrite
		bool				_readbackReady = false;
	};
}
