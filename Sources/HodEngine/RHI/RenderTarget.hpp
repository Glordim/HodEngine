#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <cstddef>
#include <cstdint>

#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline rhi
{
	class CommandBuffer;

	/// @brief Textures to draw into. Always the same ones: drawing into it every frame and reading it back
	/// on the CPU without waiting is the job of renderer::ReadbackRenderTarget.
	class HOD_RHI_API RenderTarget
	{
	public:
		RenderTarget();
		virtual ~RenderTarget();

		Vector2 GetResolution() const;

		virtual bool Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo);

		Texture* GetColorTexture() const;
		Texture* GetDepthTexture() const;

		virtual void PrepareForWrite(const CommandBuffer* commandBuffer);
		virtual void PrepareForRead(const CommandBuffer* commandBuffer);

		bool IsValid() const;

	protected:
		virtual void Clear();

	protected:
		Texture* _colorTexture = nullptr;
		Texture* _depthTexture = nullptr;

	protected:
		Vector2 _resolution; // TODO Vector2_Int ?
	};
}
