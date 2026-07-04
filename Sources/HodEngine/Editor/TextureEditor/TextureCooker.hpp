#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Renderer/Enums.hpp"

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

	protected:
		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}
