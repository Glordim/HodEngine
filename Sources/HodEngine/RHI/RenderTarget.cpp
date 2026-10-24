#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/Texture.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline rhi
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
	bool RenderTarget::Init(uint32_t width, uint32_t height, bool readback, PixelFormat colorFormat, PixelFormat depthFormat) // todo Vector2 Size
	{
		Clear();
		if (width == 0 || height == 0)
		{
			return false;
		}

		if ((colorFormat != PixelFormat::None && colorFormat != PixelFormat::R8G8B8A8_UNorm) || (depthFormat != PixelFormat::None && depthFormat != PixelFormat::D16_UNorm) ||
		    (colorFormat == PixelFormat::None && depthFormat == PixelFormat::None))
		{
			OUTPUT_ERROR("RenderTarget: needs a color format, a depth format, or both, among the ones of PixelFormat for them");
			return false;
		}

		_resolution.SetX((float)width);
		_resolution.SetY((float)height);

		if (colorFormat != PixelFormat::None)
		{
			Texture::CreateInfo createInfo;
			createInfo._width = width;
			createInfo._height = height;
			createInfo._format = colorFormat;
			createInfo._usage = TextureUsage::Sampled | TextureUsage::RenderTarget;
			if (readback)
			{
				createInfo._usage = createInfo._usage | TextureUsage::Readback;
			}

			_colorTexture = RhiDevice::GetInstance()->CreateTexture();
			if (_colorTexture->Build(createInfo) == false)
			{
				Clear();
				return false;
			}
		}

		if (depthFormat != PixelFormat::None)
		{
			Texture::CreateInfo createInfo;
			createInfo._width = width;
			createInfo._height = height;
			createInfo._format = depthFormat;
			createInfo._usage = TextureUsage::RenderTarget;

			_depthTexture = RhiDevice::GetInstance()->CreateTexture();
			if (_depthTexture->Build(createInfo) == false)
			{
				Clear();
				return false;
			}
		}

		_colorFormat = colorFormat;
		_depthFormat = depthFormat;
		return true;
	}

	/// @brief
	void RenderTarget::Clear()
	{
		DefaultAllocator::GetInstance().Delete(_colorTexture);
		_colorTexture = nullptr;

		DefaultAllocator::GetInstance().Delete(_depthTexture);
		_depthTexture = nullptr;

		_colorFormat = PixelFormat::None;
		_depthFormat = PixelFormat::None;
	}

	/// @brief
	/// @return
	Texture* RenderTarget::GetColorTexture() const
	{
		return _colorTexture;
	}

	/// @brief
	/// @return
	Texture* RenderTarget::GetDepthTexture() const
	{
		return _depthTexture;
	}

	/// @brief
	/// @return
	PixelFormat RenderTarget::GetColorFormat() const
	{
		return _colorFormat;
	}

	/// @brief
	/// @return
	PixelFormat RenderTarget::GetDepthFormat() const
	{
		return _depthFormat;
	}

	/// @brief
	/// @return
	bool RenderTarget::IsValid() const
	{
		return _colorTexture != nullptr || _depthTexture != nullptr;
	}

	/// @brief
	void RenderTarget::PrepareForWrite(const CommandBuffer* commandBuffer)
	{
		(void)commandBuffer;
	}

	/// @brief
	void RenderTarget::PrepareForRead(const CommandBuffer* commandBuffer)
	{
		(void)commandBuffer;
	}
}
