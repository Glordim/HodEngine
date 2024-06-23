#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "Metal/Metal.hpp"
#include <Foundation/Foundation.hpp>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalShader::MetalShader(ShaderType type) : Shader(type)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalShader::~MetalShader()
		{
            if (_function != nullptr)
            {
                _function->release();
            }
            
            if (_library != nullptr)
            {
                _library->release();
            }
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalShader::LoadInternal(const void* data, uint32_t size)
		{
            // Create the Metal library from the NSData object
            NS::Error* error = nullptr;
            RendererMetal* metalRenderer = RendererMetal::GetInstance();
            _library = metalRenderer->GetDevice()->newLibrary(dispatch_data_create(data, size, nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT), &error);
            
            NS::String* functionName = nullptr;
            switch (GetShaderType())
            {
                case ShaderType::Vertex:
                {
                    functionName = NS::String::string("VertexMain", NS::ASCIIStringEncoding);
                }
                break;

                case ShaderType::Fragment:
                {
                    functionName = NS::String::string("FragMain", NS::ASCIIStringEncoding);
                }
                break;

                case ShaderType::Geometry:
                {
                    functionName = NS::String::string("GeometryMain", NS::ASCIIStringEncoding);
                }
                break;

                case ShaderType::Compute:
                {
                    functionName = NS::String::string("ComputeMain", NS::ASCIIStringEncoding);
                }
                break;
            }
                                                      
            _function = _library->newFunction(functionName);
            functionName->release();
            
			return true;
		}
    
        MTL::Library* MetalShader::GetNativeLibrary() const
        {
            return _library;
        }
    
        MTL::Function* MetalShader::GetNativeFunction() const
        {
            return _function;
        }
	}
}
