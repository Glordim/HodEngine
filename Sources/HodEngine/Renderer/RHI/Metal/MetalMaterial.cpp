#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
//#include "HodEngine/Renderer/RHI/Metal/MetalVertexInput.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <Metal/Metal.hpp>

#include <iostream> // TODO output

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterial::MetalMaterial() : Material()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterial::~MetalMaterial()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalMaterial::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
            MTL::Function* vertexFunction = static_cast<MetalShader*>(vertexShader)->GetNativeFunction();
            MTL::Function* fragmentFunction = static_cast<MetalShader*>(fragmentShader)->GetNativeFunction();
            //MTL::VertexDescriptor* vertexDescriptor = static_cast<MetalVertexInput*>(vertexInputs)->GetNativeVertexDescriptor();
            
            MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

            uint32_t vertexStructIndex = 0;
            uint32_t offset = 0;
            
            NS::Array* stageInputs = vertexFunction->stageInputAttributes();
            for (NS::UInteger i = 0; i < stageInputs->count(); ++i)
            {
                MTL::Attribute* attribute = static_cast<MTL::Attribute*>(stageInputs->object(i));
                //std::cout << "Attribute at index " << attr->attributeIndex() << ": " << attr->name()->utf8String() << std::endl;
                //std::cout << "  Format: " << attr->format() << std::endl;
                
                std::cout << attribute->name()->utf8String() << std::endl;
                std::cout << attribute->attributeType() << std::endl;
                
                MTL::VertexAttributeDescriptor* vertexAttribute = MTL::VertexAttributeDescriptor::alloc()->init();
                vertexAttribute->setOffset(offset);
                vertexAttribute->setBufferIndex(0);
                
                if (attribute->attributeType() == MTL::DataTypeFloat2)
                {
                    vertexAttribute->setFormat(MTL::VertexFormatFloat2);
                    offset += sizeof(float) * 2;
                }
                else if (attribute->attributeType() == MTL::DataTypeFloat4)
                {
                    vertexAttribute->setFormat(MTL::VertexFormatFloat4);
                    offset += sizeof(float) * 4;
                }
                
                vertexDescriptor->attributes()->setObject(vertexAttribute, i);
            }
            MTL::VertexBufferLayoutDescriptor* layout = MTL::VertexBufferLayoutDescriptor::alloc()->init();
            layout->setStride(offset);
            vertexDescriptor->layouts()->setObject(layout, 0);
            
            // Create a pipeline descriptor
            MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
            pipelineDescriptor->setVertexFunction(vertexFunction);
            pipelineDescriptor->setFragmentFunction(fragmentFunction);
            pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
            pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

            // Create the render pipeline state
            NS::Error* pipelineError = nil;
            RendererMetal* rendererMetal = RendererMetal::GetInstance();
            _renderPipelineState = rendererMetal->GetDevice()->newRenderPipelineState(pipelineDescriptor, &pipelineError);
            if (_renderPipelineState == nullptr)
            {
                std::cerr << "Failed to create render pipeline state: " << pipelineError->localizedDescription()->utf8String() << std::endl; // TODO Output
                return false;
            }
            
			return true;
		}
    
        MTL::RenderPipelineState* MetalMaterial::GetNativeRenderPipeline() const
        {
            return _renderPipelineState;
        }
	}
}
