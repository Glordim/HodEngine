#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Math/Matrix4.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"

#include <cstdint>

namespace hod::inline rhi
{
	class BindGroup;
	class Buffer;
	class CommandBuffer;
	class Texture;
}

namespace hod::inline renderer
{
	class Material;

	/// @brief The values of the parameters of a Material: uniforms and textures, addressed by name.
	/// The values live here, on the CPU. Bind copies them into the uniform data of the current frame,
	/// so changing a value never touches what a frame still in flight reads.
	class HOD_RENDERER_API MaterialInstance
	{
	public:
		/// @return nullptr if there is no material
		static MaterialInstance* Create(const Material* material);

		explicit MaterialInstance(const Material& material);
		MaterialInstance(const MaterialInstance&) = delete;
		MaterialInstance(MaterialInstance&&) = delete;
		~MaterialInstance();

		void operator=(const MaterialInstance&) = delete;
		void operator=(MaterialInstance&&) = delete;

	public:
		const Material& GetMaterial() const;

		// memberName: path of a uniform, like "ubo.color" (see Material::FindUniform).
		// Ignored if the material has no such uniform, or if it does not have the size of the value.
		void SetInt(const String& memberName, int value);
		void SetFloat(const String& memberName, float value);
		void SetVec2(const String& memberName, const Vector2& value);
		void SetVec4(const String& memberName, const Vector4& value);
		void SetMat4(const String& memberName, const Matrix4& value);

		// A null (or not yet built) texture draws as a white one.
		// The sampler declared as "<memberName>Sampler", if any, follows the texture.
		void SetTexture(const String& memberName, const rhi::Texture* value);

		// Binds the pipeline of the material and the sets [setOffset, setOffset + setCount) with the current values
		void Bind(CommandBuffer& commandBuffer, uint32_t setOffset = 0, uint32_t setCount = UINT32_MAX) const;

	private:
		static constexpr uint32_t MaxUniformBlocksPerSet = 8;

		struct TextureSlot
		{
			const rhi::Texture* _texture = nullptr;
			bool           _set = false; // SetTexture was called for it
		};

		// The BindGroup made for a given list of uniform buffers: one per uniform buffer page the set was uploaded to
		struct BoundGroup
		{
			Vector<Buffer*> _uniformBuffers;
			BindGroup*      _bindGroup = nullptr;
		};

		struct Set
		{
			Vector<Vector<uint8_t>> _uniformBlocks; // one per uniform block of the set
			Vector<TextureSlot>     _textures;      // one per texture block of the set

			mutable Vector<BoundGroup> _bindGroups;
		};

	private:
		void SetUniform(const String& path, const void* value, uint32_t size);
		void SetTextureSlot(const String& name, const rhi::Texture* texture);

		BindGroup* GetBindGroup(uint32_t setIndex, Buffer* const* uniformBuffers, uint32_t uniformBufferCount) const;
		void       ReleaseBindGroups(Set& set);

	private:
		const Material& _material;
		Vector<Set>     _sets; // indexed by set
	};
}
