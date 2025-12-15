#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/String.hpp"
#include <iostream>

namespace hod::renderer
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

		uint32_t frameInFlight = Renderer::GetInstance()->GetFrameInFlightCount();

		_colorTextures.Resize(frameInFlight, nullptr);
		_depthTextures.Resize(frameInFlight, nullptr);
		for (uint32_t i = 0; i < frameInFlight; ++i)
		{
			_colorTextures[i] = Renderer::GetInstance()->CreateTexture();
			if (_colorTextures[i]->BuildColor(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}

			_depthTextures[i] = Renderer::GetInstance()->CreateTexture();
			if (_depthTextures[i]->BuildDepth(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}
		}

		return true;
	}

	/// @brief
	void RenderTarget::Clear()
	{
		for (Texture* texture : _colorTextures)
		{
			DefaultAllocator::GetInstance().Delete(texture);
		}
		_colorTextures.Clear();

		for (Texture* texture : _depthTextures)
		{
			DefaultAllocator::GetInstance().Delete(texture);
		}
		_depthTextures.Clear();
	}

	/// @brief
	/// @return
	Texture* RenderTarget::GetColorTexture() const
	{
		if (_colorTextures.Empty() == false)
		{
			return _colorTextures[Renderer::GetInstance()->GetFrameIndex()];
		}
		return nullptr;
	}

	/// @brief
	/// @return
	Texture* RenderTarget::GetDepthTexture() const
	{
		if (_depthTextures.Empty() == false)
		{
			return _depthTextures[Renderer::GetInstance()->GetFrameIndex()];
		}
		return nullptr;
	}

	/// @brief
	/// @return
	bool RenderTarget::IsValid() const
	{
		return _colorTextures.Empty() == false;
	}
}
