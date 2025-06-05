#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <unordered_map>
#include "HodEngine/Core/Vector.hpp"

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

			uint32_t	_binding;
			std::string	_name;
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
				std::string						_name;
				size_t							_size;
				size_t							_count;
				size_t							_offset;
				MemberType						_memberType;

				std::unordered_map<std::string, Member>	_childsMap;
			};

			Member	_rootMember;
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

			Type	_type;
		};

	public:

											ShaderSetDescriptor();
		virtual								~ShaderSetDescriptor();

		const Vector<BlockUbo>&		GetUboBlocks() const;
		const Vector<BlockTexture>&	GetTextureBlocks() const;

	protected:

		Vector<BlockUbo>				_uboBlockVector;
		Vector<BlockTexture>			_textureBlockVector;
	};
}
