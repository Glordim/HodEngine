#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(TextureInfo, void)
	{
		ADD_PROPERTY(TextureInfo, _offset);
		ADD_PROPERTY(TextureInfo, _size);
	}

	DESCRIBE_REFLECTED_CLASS(TextureResource, Resource)
	{
		ADD_PROPERTY(TextureResource, _width);
		ADD_PROPERTY(TextureResource, _height);
		ADD_PROPERTY(TextureResource, _componentCount);
		//ADD_PROPERTY(TextureResource, _textureInfos);
	}
}
