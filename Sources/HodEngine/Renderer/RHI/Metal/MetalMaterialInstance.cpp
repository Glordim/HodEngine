#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterialInstance::MetalMaterialInstance(const Material& material)
			: MaterialInstance(material)
		{
			
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterialInstance::~MetalMaterialInstance()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalMaterialInstance::ApplyInt(const std::string& memberName, int value)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalMaterialInstance::ApplyFloat(const std::string& memberName, float value)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalMaterialInstance::ApplyVec4(const std::string& memberName, const Vector4& value)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalMaterialInstance::ApplyMat4(const std::string& memberName, const Matrix4& value)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalMaterialInstance::ApplyTexture(const std::string& name, const Texture& value)
		{
		}
	}
}
