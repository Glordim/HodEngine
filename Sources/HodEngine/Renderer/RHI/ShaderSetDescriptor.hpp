#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <unordered_map>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API ShaderSetDescriptor
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
		virtual ~ShaderSetDescriptor() = default;

		void Merge(const ShaderSetDescriptor& other);

		const Vector<BlockUbo>&     GetUboBlocks() const;
		const Vector<BlockTexture>& GetTextureBlocks() const;

	protected:
		Vector<BlockUbo>     _uboBlockVector;
		Vector<BlockTexture> _textureBlockVector;
	};
}
