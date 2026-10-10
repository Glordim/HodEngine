#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/ReadbackRenderTarget.hpp"

#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/Texture.hpp"

namespace hod::inline renderer
{
	/// @brief
	ReadbackRenderTarget::~ReadbackRenderTarget()
	{
		Clear();
	}

	/// @brief
	/// @param width
	/// @param height
	/// @return
	bool ReadbackRenderTarget::Init(uint32_t width, uint32_t height)
	{
		Clear();

		Texture::CreateInfo createInfo;
		createInfo._allowReadWrite = true;

		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		uint32_t frameInFlightCount = rhiDevice->GetFrameInFlightCount();
		for (uint32_t index = 0; index < frameInFlightCount; ++index)
		{
			RenderTarget* renderTarget = rhiDevice->CreateRenderTarget();
			_renderTargets.PushBack(renderTarget);

			if (renderTarget->Init(width, height, createInfo) == false)
			{
				Clear();
				return false;
			}
		}

		return true;
	}

	/// @brief
	void ReadbackRenderTarget::Clear()
	{
		for (RenderTarget* renderTarget : _renderTargets)
		{
			DefaultAllocator::GetInstance().Delete(renderTarget);
		}
		_renderTargets.Clear();
	}

	/// @brief
	/// @return
	bool ReadbackRenderTarget::IsValid() const
	{
		return _renderTargets.Empty() == false;
	}

	/// @brief
	/// @return
	Vector2 ReadbackRenderTarget::GetResolution() const
	{
		if (_renderTargets.Empty())
		{
			return Vector2(0.0f, 0.0f);
		}
		return _renderTargets[0]->GetResolution();
	}

	/// @brief The frames go through the targets in turn: the one of the current frame was last drawn GetFrameInFlightCount frames ago,
	/// by a frame the Renderer waited for before starting this one
	/// @return
	RenderTarget* ReadbackRenderTarget::GetRenderTarget() const
	{
		if (_renderTargets.Empty())
		{
			return nullptr;
		}
		return _renderTargets[RhiDevice::GetInstance()->GetFrameIndex() % _renderTargets.Size()];
	}

	/// @brief
	/// @return
	Texture* ReadbackRenderTarget::GetColorTexture() const
	{
		RenderTarget* renderTarget = GetRenderTarget();
		return renderTarget != nullptr ? renderTarget->GetColorTexture() : nullptr;
	}

	/// @brief
	/// @param position
	/// @return
	Color ReadbackRenderTarget::ReadPixel(const Vector2& position) const
	{
		Texture* colorTexture = GetColorTexture();
		if (colorTexture == nullptr)
		{
			return Color(0.0f, 0.0f, 0.0f, 0.0f);
		}
		return colorTexture->ReadPixel(position);
	}
}
