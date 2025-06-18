#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"
#include "HodEngine/Renderer/Font/Font.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <cmath>
#include <cstring>

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(FontResource, reflectionDescriptor)
	{
	}

	/// @brief 
	FontResource::~FontResource()
	{
		DefaultAllocator::GetInstance().Delete(_font);
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool FontResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
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

		uint8_t* data = DefaultAllocator::GetInstance().Allocate<uint8_t>(dataSize);
		if (FileSystem::GetInstance()->Read(fileHandle, (char*)data, dataSize) != dataSize)
		{
			DefaultAllocator::GetInstance().Free(data);
			return false;
		}

		_font = DefaultAllocator::GetInstance().New<Font>();
		if (_font->LoadFromMemory(data, dataSize) == false)
		{
			DefaultAllocator::GetInstance().Free(data);
			DefaultAllocator::GetInstance().Delete(_font);
			_font = nullptr;
			return false;
		}

		DefaultAllocator::GetInstance().Free(data);
		return true;
	}

	Font* FontResource::GetFont() const
	{
		return _font;
	}
}
