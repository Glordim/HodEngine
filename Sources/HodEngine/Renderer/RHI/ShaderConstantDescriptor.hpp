#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API ShaderConstantDescriptor
	{
	public:

											ShaderConstantDescriptor();
		virtual								~ShaderConstantDescriptor();
	};
}
