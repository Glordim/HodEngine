#pragma once

#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MetalMaterialInstance : public MaterialInstance
		{
		public:

											MetalMaterialInstance(const Material& material);
											~MetalMaterialInstance() override;

		protected:

			void							ApplyInt(const std::string& memberName, int value) override;
			void							ApplyFloat(const std::string& memberName, float value) override;
			void							ApplyVec4(const std::string& memberName, const Vector4& value) override;
			void							ApplyMat4(const std::string& memberName, const Matrix4& value) override;
			void							ApplyTexture(const std::string& name, const Texture& value) override;
		};
	}
}
