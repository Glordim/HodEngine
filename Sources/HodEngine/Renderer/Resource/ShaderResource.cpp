#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/ShaderResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(ShaderResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &ShaderResource::_type, "_type");
		AddPropertyT(reflectionDescriptor, &ShaderResource::_source, "_source");
	}

	/// @brief 
	/// @return 
	ShaderResource::~ShaderResource()
	{
		DefaultAllocator::GetInstance().Delete(_shader);
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool ShaderResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		const Document::Node* dataOffsetNode = documentNode.GetChild("DataOffset");
		if (dataOffsetNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t initialStreamPos = FileSystem::GetInstance()->GetOffset(fileHandle);
		uint32_t dataOffset = dataOffsetNode->GetUInt32();

		const Document::Node* dataSizeNode = documentNode.GetChild("DataSize");
		if (dataSizeNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t dataSize = dataSizeNode->GetUInt32();

		FileSystem::GetInstance()->Seek(fileHandle, initialStreamPos + dataOffset, FileSystem::SeekMode::Begin);

		char* data = DefaultAllocator::GetInstance().Allocate<char>(dataSize);
		if (FileSystem::GetInstance()->Read(fileHandle, data, dataSize) != dataSize)
		{
			DefaultAllocator::GetInstance().Free(data);
			return false;
		}

		_shader = Renderer::GetInstance()->CreateShader(_type);
		bool loaded = _shader->LoadFromIR(data, dataSize);
		DefaultAllocator::GetInstance().Free(data);

		if (loaded == false)
		{
			DefaultAllocator::GetInstance().Delete(_shader);
			_shader = nullptr;
			return false;
		}
		return true;
	}

	/// @brief 
	/// @return 
	Shader* ShaderResource::GetShader() const
	{
		return _shader;
	}
}
