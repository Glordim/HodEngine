#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	void ShaderSetDescriptor::Merge(const ShaderSetDescriptor& other)
	{
		for (const BlockUbo& otherUboBlock : other._uboBlockVector)
		{
			bool alreadyExist = false;
			for (BlockUbo& exisintgUboBlock : _uboBlockVector)
			{
				if (otherUboBlock._binding == exisintgUboBlock._binding) // Todo binding is enought or check name too can be useful ?
				{
					alreadyExist = true;
					break;
				}
			}
			if (alreadyExist == false)
			{
				_uboBlockVector.push_back(otherUboBlock);
			}
		}

		for (const BlockTexture& otherTextureBlock : other._textureBlockVector)
		{
			bool alreadyExist = false;
			for (BlockTexture& exisintgTextureBlock : _textureBlockVector)
			{
				if (otherTextureBlock._binding == exisintgTextureBlock._binding) // Todo binding is enought or check name too can be useful ?
				{
					alreadyExist = true;
					break;
				}
			}
			if (alreadyExist == false)
			{
				_textureBlockVector.push_back(otherTextureBlock);
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const Vector<ShaderSetDescriptor::BlockUbo>& ShaderSetDescriptor::GetUboBlocks() const
	{
		return _uboBlockVector;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const Vector<ShaderSetDescriptor::BlockTexture>& ShaderSetDescriptor::GetTextureBlocks() const
	{
		return _textureBlockVector;
	}
}
