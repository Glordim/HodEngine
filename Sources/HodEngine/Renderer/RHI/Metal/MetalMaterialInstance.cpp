#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/Metal/Metal.hpp"

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
            /*
            static_cast<MetalMaterial*>(material);
            MTL::RenderPipelineState* pipelineState = 
             */
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
    
        void MetalMaterialInstance::FillCommandEncoder(MTL::RenderCommandEncoder* renderCommandEncoder) const
        {
            const MetalMaterial& material = static_cast<const MetalMaterial&>(GetMaterial());
            renderCommandEncoder->setRenderPipelineState(material.GetNativeRenderPipeline());
            
            const std::map<std::string, const Texture*>& textureMap = GetTextureMap();
            for (const auto& texturePair : textureMap)
            {
                uint32_t index = 0;//material.GetTextureIndex(texturePair.first);
                
                const MetalTexture* texture = static_cast<const MetalTexture*>(texturePair.second);
                renderCommandEncoder->setFragmentTexture(texture->GetNativeTexture(), index);
                renderCommandEncoder->setFragmentSamplerState(texture->GetNativeSampler(), index);
            }
        }
	}
}
