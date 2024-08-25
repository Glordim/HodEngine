#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/Metal/Metal.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalTexture::MetalTexture() : Texture()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalTexture::~MetalTexture()
		{
            if (_texture != nullptr)
            {
                _texture->release();
            }
            
            if (_sampler != nullptr)
            {
                _sampler->release();
            }
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalTexture::BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo)
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalTexture::BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo)
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalTexture::BuildBuffer(uint32_t width, uint32_t height, const uint8_t* pixels, const CreateInfo& createInfo)
		{
            MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
            textureDescriptor->setWidth(width);
            textureDescriptor->setHeight(height);
            textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
            textureDescriptor->setTextureType(MTL::TextureType2D);
            textureDescriptor->setStorageMode(MTL::StorageModeManaged);
            textureDescriptor->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
            _texture = RendererMetal::GetInstance()->GetDevice()->newTexture(textureDescriptor);
            textureDescriptor->release();
            
            _texture->replaceRegion(MTL::Region( 0, 0, 0, width, height, 1 ), 0, pixels, width * 4);
            
            MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
            _sampler = RendererMetal::GetInstance()->GetDevice()->newSamplerState(samplerDescriptor);
            samplerDescriptor->release();

			return true;
		}

		/// @brief 
		/// @param position 
		/// @return 
		Color MetalTexture::ReadPixel(const Vector2& position) const
		{
			return Color();
		}
    
        MTL::Texture* MetalTexture::GetNativeTexture() const
        {
            return _texture;
        }
    
        MTL::SamplerState* MetalTexture::GetNativeSampler() const
        {
            return _sampler;
        }
	}
}

