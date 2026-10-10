#pragma once
#include "HodEngine/RHI/Export.hpp"

namespace hod::inline rhi
{
	class Sampler;
	class Texture;

	/// @brief Resources bound together to one set of a GraphicsPipeline: its uniform buffers and its textures.
	/// Immutable: binding other resources means creating another BindGroup (see RhiDevice::CreateBindGroup).
	/// What changes from one draw to the next is where each uniform buffer is read, given to CommandBuffer::SetBindGroup.
	/// The buffers and textures are not owned, they must outlive the last frame drawn with the BindGroup.
	class HOD_RHI_API BindGroup
	{
	public:
		/// @brief What goes in one texture block of the set: the texture of an image block, the sampler of a sampler block, both for a combined one.
		/// A null texture is replaced by the fallback texture, a null sampler by the one of the texture.
		struct TextureBinding
		{
			const Texture* _texture = nullptr;
			const Sampler* _sampler = nullptr;
		};

	public:
		BindGroup() = default;
		BindGroup(const BindGroup&) = delete;
		BindGroup(BindGroup&&) = delete;
		virtual ~BindGroup();

		BindGroup& operator=(const BindGroup&) = delete;
		BindGroup& operator=(BindGroup&&) = delete;
	};
}
