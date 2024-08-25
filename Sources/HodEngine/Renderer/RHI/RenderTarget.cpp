#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <iostream>
#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderTarget::RenderTarget()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderTarget::~RenderTarget()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t RenderTarget::GetWidth() const
		{
			return _width;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t RenderTarget::GetHeight() const
		{
			return _height;
		}

		/// @brief 
		/// @param width 
		/// @param height 
		/// @return 
		bool RenderTarget::Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo)
		{
			_width = width;
			_height = height;

			if (_width == 0 || _height == 0)
			{
				return false;
			}

			_color = Renderer::GetInstance()->CreateTexture();
			if (_color->BuildColor(_width, _height, createInfo) == false)
			{
				Clear();
				return false;
			}
			_depth = Renderer::GetInstance()->CreateTexture();
			if (_depth->BuildDepth(_width, _height, createInfo) == false)
			{
				Clear();
				return false;
			}

			return true;
		}

		/// @brief 
		void RenderTarget::Clear()
		{
			delete _color;
			_color = nullptr;

			delete _depth;
			_depth = nullptr;
		}

		/// @brief 
		/// @return 
		Texture* RenderTarget::GetColorTexture() const
		{
			return _color;
		}

		/// @brief 
		/// @return 
		Texture* RenderTarget::GetDepthTexture() const
		{
			return _depth;
		}

		/// @brief 
		/// @return 
		bool RenderTarget::IsValid() const
		{
			return (_color != nullptr);
		}
	}
}
