#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

namespace MTL
{
    class RenderCommandEncoder;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API MetalMaterialInstance : public MaterialInstance
		{
		public:

											MetalMaterialInstance(const Material& material);
											~MetalMaterialInstance() override;
            
            void                            FillCommandEncoder(MTL::RenderCommandEncoder* renderCommandEncoder) const;

		protected:

			void							ApplyInt(const std::string& memberName, int value) override;
			void							ApplyFloat(const std::string& memberName, float value) override;
			void							ApplyVec4(const std::string& memberName, const Vector4& value) override;
			void							ApplyMat4(const std::string& memberName, const Matrix4& value) override;
			void							ApplyTexture(const std::string& name, const Texture& value) override;
		};
	}
}
