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

		// Only textures readable back on the CPU (picking) need one instance per frame-in-flight,
		// so that AcquireNextFrame's fence wait guarantees a completed, stall-free snapshot to read.
		// Textures only ever produced and consumed by the GPU within the same frame are safe with a
		// single instance: the layout transitions in PrepareForWrite/PrepareForRead already provide
		// the cross-frame GPU synchronization on this render target's own queue.
		uint32_t instanceCount = createInfo._allowReadWrite ? RhiDevice::GetInstance()->GetFrameInFlightCount() : 1;

		_colorTextures.Resize(instanceCount, nullptr);
		_depthTextures.Resize(instanceCount, nullptr);
		for (uint32_t i = 0; i < instanceCount; ++i)
		{
			_colorTextures[i] = RhiDevice::GetInstance()->CreateTexture();
			if (_colorTextures[i]->BuildColor(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}

			_depthTextures[i] = RhiDevice::GetInstance()->CreateTexture();
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
			return _colorTextures[RhiDevice::GetInstance()->GetFrameIndex() % _colorTextures.Size()];
		}
		return nullptr;
	}

	/// @brief
	/// @return
	Texture* RenderTarget::GetDepthTexture() const
	{
		if (_depthTextures.Empty() == false)
		{
			return _depthTextures[RhiDevice::GetInstance()->GetFrameIndex() % _depthTextures.Size()];
		}
		return nullptr;
	}

	/// @brief
	/// @return
	bool RenderTarget::IsValid() const
	{
		return _colorTextures.Empty() == false;
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
