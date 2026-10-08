#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <unordered_map>
#include "HodEngine/Core/Vector.hpp"

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API ShaderConstantDescriptor
	{
	public:

											ShaderConstantDescriptor();
		virtual								~ShaderConstantDescriptor();
	};
}
