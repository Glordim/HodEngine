#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <map>
#include <vector>

namespace spirv_cross
{
    class Compiler;
    struct Resource;
    struct SPIRType;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API DescriptorSetLayout
		{
		public:

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
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

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct BlockUbo : Block
			{
				//-----------------------------------------------------------------------------
				//! @brief		
				//-----------------------------------------------------------------------------
				struct Member
				{
					std::string						_name;
					size_t							_size;
					size_t							_count;
					size_t							_offset;

					std::map<std::string, Member>	_childsMap;
				};

				Member	_rootMember;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct BlockTexture : Block
			{
				VkDescriptorType	_type;
			};

		public:

												DescriptorSetLayout();
			virtual								~DescriptorSetLayout();

			VkDescriptorSetLayout				GetDescriptorSetLayout() const;

			void								ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource);
			void								ExtractUboSubMembers(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& type, BlockUbo::Member& member);

			void								ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource, VkDescriptorType type);

			bool								BuildDescriptorSetLayout();

			const std::vector<BlockUbo>&		GetUboBlocks() const;
			const std::vector<BlockTexture>&	GetTextureBlocks() const;

		private:

			VkDescriptorSetLayout				_descriptorSetLayout;

			std::vector<BlockUbo>				_uboBlockVector;
			std::vector<BlockTexture>			_textureBlockVector;
		};
	}
}
