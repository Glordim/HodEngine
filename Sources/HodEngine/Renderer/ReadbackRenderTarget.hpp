#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline rhi
{
	class RenderTarget;
	class Texture;
}

namespace hod::inline renderer
{
	/// @brief A target drawn every frame and read back on the CPU without waiting for the GPU, like the one used for picking.
	/// The CPU runs ahead of the GPU: reading what the current frame draws would mean waiting for it. So there is one RenderTarget
	/// per frame in flight, and what is read is the content the GPU is done with: the frame drawn GetFrameInFlightCount frames ago.
	/// Something only drawn then sampled by the GPU (a viewport, a screen in the scene) does not need this, a RenderTarget is enough.
	class HOD_RENDERER_API ReadbackRenderTarget
	{
	public:
		ReadbackRenderTarget() = default;
		ReadbackRenderTarget(const ReadbackRenderTarget&) = delete;
		ReadbackRenderTarget(ReadbackRenderTarget&&) = delete;
		~ReadbackRenderTarget();

		ReadbackRenderTarget& operator=(const ReadbackRenderTarget&) = delete;
		ReadbackRenderTarget& operator=(ReadbackRenderTarget&&) = delete;

	public:
		bool Init(uint32_t width, uint32_t height);
		void Clear();

		bool    IsValid() const;
		Vector2 GetResolution() const;

		// The target to draw the current frame into
		RenderTarget* GetRenderTarget() const;
		Texture*      GetColorTexture() const;

		// Reads the last content the GPU is done with. Must be called before the current frame is submitted (Renderer::Render),
		// since the current frame then draws over it.
		Color ReadPixel(const Vector2& position) const;

	private:
		Vector<RenderTarget*> _renderTargets; // one per frame in flight
	};
}
