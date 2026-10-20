#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"

#include "HodEngine/RHI/Enums.hpp"
#include "HodEngine/RHI/Texture.hpp"

namespace hod::inline renderer
{	
	class HOD_RENDERER_API TextureResource : public Resource
	{
		REFLECTED_CLASS(TextureResource, Resource)

	public:

							TextureResource() = default;
							TextureResource(const TextureResource&) = delete;
							TextureResource(TextureResource&&) = delete;
							~TextureResource() override;

		TextureResource&	operator = (const TextureResource&) = delete;
		TextureResource&	operator = (TextureResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		rhi::Texture*		GetTexture() const;

		// Replaces the default sampler of the texture, for its settings to be edited live.
		// The MaterialInstances it was set to (SetTexture) follow it.
		void				SetSamplerStates(FilterMode filterMode, WrapMode wrapMode);

		// Builds the texture again from pixels of its size (R8G8B8A8, the mipLevelCount levels one after the other), for its mipmaps to be edited live.
		// The MaterialInstances it was set to (SetTexture) follow it.
		bool				SetPixels(const uint8_t* pixels, uint8_t mipLevelCount);

	public:

		uint16_t					_width = 0;
		uint16_t					_height = 0;
		uint8_t						_mipLevelCount = 1;
		uint8_t						_componentCount = 0;
		FilterMode					_filterMode = FilterMode::Nearest;
		WrapMode					_wrapMode = WrapMode::Clamp;

		rhi::Texture*				_texture = nullptr;
	};
}