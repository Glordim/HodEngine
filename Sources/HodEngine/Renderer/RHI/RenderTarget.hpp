#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <cstdint>
#include <cstddef>

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::renderer
{
	class CommandBuffer;

	/// @brief 
	class HOD_RENDERER_API RenderTarget
	{
	public:

						RenderTarget();
		virtual			~RenderTarget();

		Vector2			GetResolution() const;

		virtual bool	Init(uint32_t width, uint32_t height, const Texture::CreateInfo& createInfo);

		Texture*		GetColorTexture() const;
		Texture*		GetDepthTexture() const;

		virtual void	PrepareForWrite(const CommandBuffer* commandBuffer) = 0;
		virtual void	PrepareForRead(const CommandBuffer* commandBuffer) = 0;

		bool			IsValid() const;

	protected:

		virtual void	Clear();

	protected:

		Texture*		_color = nullptr;
		Texture*		_depth = nullptr;

	protected:

		Vector2			_resolution; // TODO Vector2_Int ?
	};
}
