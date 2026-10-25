#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Text/GlyphCache.hpp"
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
		ForgetGlyphs();
	}

	/// @brief The ones of the face as it is: before it is destroyed or loaded again
	void FontResource::ForgetGlyphs()
	{
		Renderer* renderer = Renderer::GetInstance();
		if (renderer != nullptr && renderer->GetGlyphCache() != nullptr)
		{
			renderer->GetGlyphCache()->Forget(_fontFace);
		}
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

		ForgetGlyphs();
		const bool loaded = _fontFace.LoadFromMemory(buffer, ttfDataBlock->_uncompressedSize); // copies it
		DefaultAllocator::GetInstance().Free(buffer);
		if (loaded == false)
		{
			OUTPUT_ERROR("FontResource::Initialize: load font failed");
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	const FontFace& FontResource::GetFontFace() const
	{
		return _fontFace;
	}
}
