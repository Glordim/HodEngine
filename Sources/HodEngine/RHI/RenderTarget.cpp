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
	bool RenderTarget::Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo) // todo Vector2 Size
	{
		Clear();
		if (width == 0 || height == 0)
		{
			return false;
		}

		_resolution.SetX((float)width);
		_resolution.SetY((float)height);

		_colorTexture = RhiDevice::GetInstance()->CreateTexture();
		if (_colorTexture->BuildColor(width, height, createInfo) == false)
		{
			Clear();
			return false;
		}

		_depthTexture = RhiDevice::GetInstance()->CreateTexture();
		if (_depthTexture->BuildDepth(width, height, createInfo) == false)
		{
			Clear();
			return false;
		}

		return true;
	}

	/// @brief
	void RenderTarget::Clear()
	{
		DefaultAllocator::GetInstance().Delete(_colorTexture);
		_colorTexture = nullptr;

		DefaultAllocator::GetInstance().Delete(_depthTexture);
		_depthTexture = nullptr;
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
	bool RenderTarget::IsValid() const
	{
		return _colorTexture != nullptr;
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
