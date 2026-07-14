#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/Font.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include <cmath>
#include <cstring>

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(FontResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
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
	bool FontResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* ttfDataBlock = resourceContainer.FindDataBlock("ttf");
		if (ttfDataBlock == nullptr)
		{
			OUTPUT_ERROR("FontResource::Initialize: missing 'ttf' data block");
			return false;
		}

		uint8_t* buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfDataBlock->_uncompressedSize);
		if (ttfDataBlock->_stream->Read(buffer, ttfDataBlock->_uncompressedSize) != ttfDataBlock->_uncompressedSize)
		{
			OUTPUT_ERROR("FontResource::Initialize: can't read 'ttf' data block");
			DefaultAllocator::GetInstance().Free(buffer);
			return false;
		}

		_font = DefaultAllocator::GetInstance().New<Font>();
		if (_font->LoadFromMemory(buffer, ttfDataBlock->_uncompressedSize) == false)
		{
			OUTPUT_ERROR("FontResource::Initialize: load font failed");
			DefaultAllocator::GetInstance().Delete(_font);
			_font = nullptr;
			DefaultAllocator::GetInstance().Free(buffer);
			return false;
		}
		DefaultAllocator::GetInstance().Free(buffer);
		return true;
	}

	Font* FontResource::GetFont() const
	{
		return _font;
	}
}
