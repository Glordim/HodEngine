#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <unordered_map>
#include "HodEngine/Core/Vector.hpp"

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
