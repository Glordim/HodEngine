#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <cstdint>
#include <cstddef>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline rhi
{
	class CommandBuffer;
	class Sampler;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API Texture
	{
	public:

		struct CreateInfo
		{
			bool		_allowReadWrite = false;
		};

	public:

		Texture();
		virtual ~Texture();

		uint32_t	GetWidth() const;
		uint32_t	GetHeight() const;

		// The sampler to read the texture with when nothing asks for another one.
		// None by default, which leaves the choice to RhiDevice::GetSampler(Sampler::CreateInfo()). Can be changed at any time.
		void			SetDefaultSampler(const Sampler* sampler);
		const Sampler*	GetDefaultSampler() const;

		virtual bool BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) = 0;
		virtual bool BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) = 0;
		virtual bool BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) = 0;

		virtual Color ReadPixel(const Vector2& position) const = 0;

		// Records (into the given, already-recording command buffer) whatever is needed to capture
		// this texture's current content for a later ReadPixel() call. No-op for textures that were
		// not built with CreateInfo::_allowReadWrite.
		virtual void CaptureReadback(CommandBuffer* commandBuffer) = 0;

	protected:

		uint32_t	_width = 0;
		uint32_t	_height = 0;

		const Sampler*	_defaultSampler = nullptr;
	};
}
