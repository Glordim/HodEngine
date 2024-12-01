#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <string>
#include <unordered_map>
#include <vector>

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
			/// @brief 
			struct Member
			{
				std::string						_name;
				size_t							_size;
				size_t							_count;
				size_t							_offset;

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

		const std::vector<BlockUbo>&		GetUboBlocks() const;
		const std::vector<BlockTexture>&	GetTextureBlocks() const;

	protected:

		std::vector<BlockUbo>				_uboBlockVector;
		std::vector<BlockTexture>			_textureBlockVector;
	};
}
