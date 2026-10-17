#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <unordered_map>

namespace hod::inline rhi
{
	/// @brief Content of one descriptor set of a shader, built from its reflection
	class HOD_RHI_API ShaderSetDescriptor
	{
	public:
		/// @brief
		struct Block
		{
			enum Type
			{
				Ubo,
				Texture
			};

			uint32_t _binding;
			String   _name;
		};

		/// @brief
		struct BlockUbo : Block
		{
			enum MemberType
			{
				Float,
				Float2,
				Float4,
			};

			/// @brief
			struct Member
			{
				String     _name;
				size_t     _size;
				size_t     _count;
				size_t     _offset;
				MemberType _memberType;
				// [Color] in the shader (ShaderLib/Attributes.slang): edited as a color.
				// TODO Only the editors use it, the RHI has no use for it: it is here because the reflection is only read here.
				// Should the editor read the reflection itself instead (e.g. metadata the MaterialCooker extracts into a data block of its own)?
				bool       _color = false;

				std::unordered_map<String, Member> _childsMap;
			};

			Member _rootMember;
		};

		/// @brief
		struct BlockTexture : Block
		{
			enum Type
			{
				Texture,
				Sampler,
				Combined,
			};

			Type _type;
		};

	public:
		ShaderSetDescriptor() = default;
		ShaderSetDescriptor(const ShaderSetDescriptor& other) = default;
		~ShaderSetDescriptor() = default;

		void Merge(const ShaderSetDescriptor& other);

		// variableNode: a shader parameter, or a field of a ParameterBlock, of the reflection
		void ExtractBlockUbo(const DocumentNode& variableNode, uint32_t binding);
		void ExtractBlockTexture(const DocumentNode& variableNode, uint32_t binding);
		void ExtractBlockSampler(const DocumentNode& variableNode, uint32_t binding);

		// Where the set is for the graphics API: its descriptor set on Vulkan, its register space on D3D12.
		// Not always the number of the set, see Shader::GenerateDescriptors.
		uint32_t GetSpace() const;
		void     SetSpace(uint32_t space);

		const Vector<BlockUbo>&     GetUboBlocks() const;
		const Vector<BlockTexture>& GetTextureBlocks() const;

	private:
		void ExtractUboSubMembers(const DocumentNode& fieldNode, BlockUbo::Member& structMember);

	private:
		Vector<BlockUbo>     _uboBlockVector;
		Vector<BlockTexture> _textureBlockVector;
		uint32_t             _space = 0;
	};
}
