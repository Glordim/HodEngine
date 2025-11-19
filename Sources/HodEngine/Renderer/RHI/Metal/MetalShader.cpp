#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include "Metal/Metal.hpp"
#include <Foundation/Foundation.hpp>

namespace hod::renderer
{
	/// @brief
	/// @param type
	MetalShader::MetalShader(ShaderType type)
	: Shader(type)
	{
	}

	/// @brief
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

	/// @brief
	/// @param data
	/// @param Size
	/// @return
	bool MetalShader::LoadFromIR(const void* data, uint32_t Size)
	{
		RendererMetal* metalRenderer = RendererMetal::GetInstance();

		NS::Error* error = nullptr;
		_library = metalRenderer->GetDevice()->newLibrary(dispatch_data_create(data, Size, nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT), &error);

		if (_library == nullptr)
		{
			OUTPUT_ERROR("MetalShader::LoadFromSource fail: {}", error->description()->utf8String());
			return false;
		}

		return FindFunction();
	}

	/// @brief
	/// @return
	bool MetalShader::FindFunction()
	{
		if (_library == nullptr)
		{
			OUTPUT_ERROR("MetalShader::FindFunction fail: Invalid MtlLibrary");
			return false;
		}

		const char* functionName = nullptr;
		switch (GetShaderType())
		{
			case ShaderType::Vertex:
			{
				functionName = "VertexMain";
			}
			break;

			case ShaderType::Fragment:
			{
				functionName = "FragmentMain";
			}
			break;

			case ShaderType::Geometry:
			{
				functionName = "GeometryMain";
			}
			break;

			case ShaderType::Compute:
			{
				functionName = "ComputeMain";
			}
			break;
		}

		NS::String* nsFunctionName = NS::String::string(functionName, NS::ASCIIStringEncoding);
		_function = _library->newFunction(nsFunctionName);
		nsFunctionName->release();

		if (_function == nullptr)
		{
			OUTPUT_ERROR("MetalShader::FindFunction fail: Unable to find {} function in MtlLibrary", functionName);
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	MTL::Library* MetalShader::GetNativeLibrary() const
	{
		return _library;
	}

	/// @brief
	/// @return
	MTL::Function* MetalShader::GetNativeFunction() const
	{
		return _function;
	}

	/// @brief
	/// @return
	bool MetalShader::GenerateDescriptors()
	{
		return false;
	}
}
