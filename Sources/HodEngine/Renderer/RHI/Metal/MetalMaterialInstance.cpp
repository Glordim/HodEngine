#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

#include <Metal/Metal.hpp>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		/// @param material 
		MetalMaterialInstance::MetalMaterialInstance(const Material& material)
			: MaterialInstance(material)
		{
            /*
            static_cast<MetalMaterial*>(material);
            MTL::RenderPipelineState* pipelineState = 
             */
		}

		/// @brief 
		MetalMaterialInstance::~MetalMaterialInstance()
		{

		}

		/// @brief 
		/// @param memberName 
		/// @param value 
		void MetalMaterialInstance::ApplyInt(const String& memberName, int value)
		{
			// TODO
			(void)memberName;
			(void)value;
		}

		/// @brief 
		/// @param memberName 
		/// @param value 
		void MetalMaterialInstance::ApplyFloat(const String& memberName, float value)
		{
			// TODO
			(void)memberName;
			(void)value;
		}

		/// @brief 
		/// @param memberName 
		/// @param value 
		void MetalMaterialInstance::ApplyVec2(const String& memberName, const Vector2& value)
		{
			// TODO
			(void)memberName;
			(void)value;
		}

		/// @brief 
		/// @param memberName 
		/// @param value 
		void MetalMaterialInstance::ApplyVec4(const String& memberName, const Vector4& value)
		{
			// TODO
			(void)memberName;
			(void)value;
		}

		/// @brief 
		/// @param memberName 
		/// @param value 
		void MetalMaterialInstance::ApplyMat4(const String& memberName, const Matrix4& value)
		{
			// TODO
			(void)memberName;
			(void)value;
		}

		/// @brief 
		/// @param name 
		/// @param value 
		void MetalMaterialInstance::ApplyTexture(const String& name, const Texture& value)
		{
			// TODO
			(void)name;
			(void)value;
		}
    
        /// @brief 
        /// @param renderCommandEncoder 
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
