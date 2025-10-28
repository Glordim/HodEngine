#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/String.hpp"
#include <iostream>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		RenderTarget::RenderTarget() {}

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		RenderTarget::~RenderTarget()
		{
			Clear();
		}

		/// @brief
		/// @return
		Vector2 RenderTarget::GetResolution() const
		{
			return _resolution;
		}

		/// @brief
		/// @param width
		/// @param height
		/// @return
		bool RenderTarget::Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo) // todo Vector2 Size
		{
			if (width == 0 || height == 0)
			{
				return false;
			}

			_resolution.SetX((float)width);
			_resolution.SetY((float)height);

			_color = Renderer::GetInstance()->CreateTexture();
			if (_color->BuildColor(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}
			_depth = Renderer::GetInstance()->CreateTexture();
			if (_depth->BuildDepth(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}

			return true;
		}

		/// @brief
		void RenderTarget::Clear()
		{
			DefaultAllocator::GetInstance().Delete(_color);
			_color = nullptr;

			DefaultAllocator::GetInstance().Delete(_depth);
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
			return _color != nullptr;
		}
	}
}
