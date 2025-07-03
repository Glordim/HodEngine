#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(MaterialResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialResource::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialResource::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialResource::_defaultInstanceParams, "_defaultInstanceParams");
	}

	/// @brief 
	/// @return 
	MaterialResource::~MaterialResource()
	{
		DefaultAllocator::GetInstance().Delete(_material);
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool MaterialResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		uint32_t fileOffset = FileSystem::GetInstance()->GetOffset(fileHandle);

		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		const hod::Document::Node* vertexDataOffset = documentNode.GetChild("VertexDataOffset");
		const hod::Document::Node* vertexDataSize = documentNode.GetChild("VertexDataSize");
		const hod::Document::Node* fragmentDataOffset = documentNode.GetChild("FragmentDataOffset");
		const hod::Document::Node* fragmentDataSize = documentNode.GetChild("FragmentDataSize");
		if (vertexDataOffset == nullptr || vertexDataSize == nullptr || fragmentDataOffset == nullptr || fragmentDataOffset == nullptr)
		{
			return false;
		}

		uint32_t vertexShaderSize = vertexDataSize->GetUInt32();
		void* vertexShaderBytecode = DefaultAllocator::GetInstance().Allocate(vertexShaderSize);
		FileSystem::GetInstance()->Seek(fileHandle, fileOffset + vertexDataOffset->GetUInt32(), FileSystem::SeekMode::Begin);
		if (FileSystem::GetInstance()->Read(fileHandle, vertexShaderBytecode, vertexShaderSize) != vertexShaderSize)
		{
			DefaultAllocator::GetInstance().Free(vertexShaderBytecode);
			return false;
		}
		_vertexShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Vertex);
		if (_vertexShader->LoadFromIR(vertexShaderBytecode, vertexShaderSize) == false)
		{
			DefaultAllocator::GetInstance().Delete(_vertexShader);
			DefaultAllocator::GetInstance().Free(vertexShaderBytecode);
			_vertexShader = nullptr;
			return false;
		}
		DefaultAllocator::GetInstance().Free(vertexShaderBytecode);

		uint32_t fragmentShaderSize = fragmentDataSize->GetUInt32();
		void* fragmentShaderBytecode = DefaultAllocator::GetInstance().Allocate(fragmentShaderSize);
		FileSystem::GetInstance()->Seek(fileHandle, fileOffset + fragmentDataOffset->GetUInt32(), FileSystem::SeekMode::Begin);
		if (FileSystem::GetInstance()->Read(fileHandle, fragmentShaderBytecode, fragmentShaderSize) != fragmentShaderSize)
		{
			DefaultAllocator::GetInstance().Free(fragmentShaderBytecode);
			return false;
		}
		_fragmentShader = Renderer::GetInstance()->CreateShader(Shader::ShaderType::Fragment);
		if (_fragmentShader->LoadFromIR(fragmentShaderBytecode, fragmentShaderSize) == false)
		{
			DefaultAllocator::GetInstance().Delete(_fragmentShader);
			DefaultAllocator::GetInstance().Free(fragmentShaderBytecode);
			_fragmentShader = nullptr;
			return false;
		}
		DefaultAllocator::GetInstance().Free(fragmentShaderBytecode);

		Vector<VertexInput> vertexInputs;
		vertexInputs.push_back(VertexInput(0, 0, VertexInput::Format::R32G32_SFloat));
		vertexInputs.push_back(VertexInput(1, 8, VertexInput::Format::R32G32_SFloat));

		_material = Renderer::GetInstance()->CreateMaterial(vertexInputs.data(), (uint32_t)vertexInputs.size(), _vertexShader, _fragmentShader, _polygonMode, _topololy, false);
		if (_material == nullptr)
		{
			return false;
		}

		_material->CreateDefaultInstance();
		MaterialSerializationHelper::ApplyParamsFromDocument(*const_cast<MaterialInstance*>(_material->GetDefaultInstance()), _defaultInstanceParams.GetRootNode(), _textureResources);

		return true;
	}

	/// @brief 
	/// @return 
	Material* MaterialResource::GetMaterial() const
	{
		return _material;
	}
}
