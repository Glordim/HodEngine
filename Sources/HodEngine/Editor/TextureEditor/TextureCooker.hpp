#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/RHI/Enums.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API TextureSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(TextureSettings)

	public:
		bool       _generateMipmap = false;
		FilterMode _filterMode = FilterMode::Linear;
		WrapMode   _wrapMode = WrapMode::Clamp;
	};

	class HOD_EDITOR_API TextureCooker : public Cooker
	{
	public:
		TextureCooker();
		TextureCooker(const TextureCooker&) = delete;
		TextureCooker(TextureCooker&&) = delete;
		~TextureCooker() override = default;

		TextureCooker& operator=(const TextureCooker&) = delete;
		TextureCooker& operator=(TextureCooker&&) = delete;

	public:
		// The pixels as TextureResource reads them (R8G8B8A8, the levels one after the other): the full mip chain if the settings ask for it, else the pixels alone.
		// The levels are filtered from the previous one, the edges wrapped as the texture is sampled
		// @return the number of levels, 0 if it failed
		static uint8_t BuildMipChain(const uint8_t* pixels, uint16_t width, uint16_t height, const TextureSettings& textureSettings, Vector<uint8_t>& mipChain);

	protected:
		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}
