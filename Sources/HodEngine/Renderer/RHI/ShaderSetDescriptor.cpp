#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	/// @brief 
	ShaderSetDescriptor::ShaderSetDescriptor()
	{
	}

	/// @brief 
	ShaderSetDescriptor::~ShaderSetDescriptor()
	{
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
