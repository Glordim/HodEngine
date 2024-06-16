#pragma once

#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace MTL
{
    class Texture;
    class SamplerState;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MetalTexture : public Texture
		{
		public:

							MetalTexture();
							~MetalTexture() override;

			bool			BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
			bool			BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
			bool			BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) override;

			Color			ReadPixel(const Vector2& position) const override;
            
            MTL::Texture*       GetNativeTexture() const;
            MTL::SamplerState*  GetNativeSampler() const;
            
        private:
            
            MTL::Texture*       _texture = nullptr;
            MTL::SamplerState*  _sampler = nullptr;
		};
	}
}
