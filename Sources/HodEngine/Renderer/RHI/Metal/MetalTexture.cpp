#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

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
			return true;
		}

		/// @brief 
		/// @param position 
		/// @return 
		Color MetalTexture::ReadPixel(const Vector2& position) const
		{
			return Color();
		}
	}
}

