#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/Font.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

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
	bool FontResource::Initialize(const ResourceContainer& /*resourceContainer*/)
	{
		return false;
		/*
		(void)documentNode; // TODO

		if (datas.Empty())
		{
			OUTPUT_ERROR("FontResource::Initialize: invalid data count");
			return false;
		}

		const Resource::Data& data = datas[0];

		_font = DefaultAllocator::GetInstance().New<Font>();
		if (_font->LoadFromMemory(data._buffer, data._size) == false)
		{
			OUTPUT_ERROR("FontResource::Initialize: load font failed");
			DefaultAllocator::GetInstance().Delete(_font);
			_font = nullptr;
			return false;
		}
		return true;
		*/
	}

	Font* FontResource::GetFont() const
	{
		return _font;
	}
}
