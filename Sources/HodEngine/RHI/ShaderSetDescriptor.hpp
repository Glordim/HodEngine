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

		void ExtractBlockUbo(const DocumentNode& parameterNode);
		void ExtractBlockTexture(const DocumentNode& parameterNode);
		void ExtractBlockSampler(const DocumentNode& parameterNode);

		const Vector<BlockUbo>&     GetUboBlocks() const;
		const Vector<BlockTexture>& GetTextureBlocks() const;

	private:
		void ExtractUboSubMembers(const DocumentNode& fieldNode, BlockUbo::Member& structMember);

	private:
		Vector<BlockUbo>     _uboBlockVector;
		Vector<BlockTexture> _textureBlockVector;
	};
}
