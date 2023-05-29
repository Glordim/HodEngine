#include "HodEngine/Renderer/Resource/TextureResource.h"

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(TextureInfo)
	{
		core::Reflection::Property::Variable* offset = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt32, offsetof(TextureInfo, _offset), "offset");
		AddProperty(offset);

		core::Reflection::Property::Variable* size = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt32, offsetof(TextureInfo, _size), "size");
		AddProperty(size);
	}

	DESCRIBE_REFLECTED_DERIVED_CLASS(TextureResource, Resource)
	{
		core::Reflection::Property::Variable* width = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt16, offsetof(TextureResource, _width), "width");
		AddProperty(width);

		core::Reflection::Property::Variable* height = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt16, offsetof(TextureResource, _height), "height");
		AddProperty(height);

		core::Reflection::Property::Variable* componentCount = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt8, offsetof(TextureResource, _componentCount), "componentCount");
		AddProperty(componentCount);

		core::Reflection::Property::Array* textureInfos = new core::Reflection::Property::Array(core::Reflection::Property::Variable::Object, offsetof(TextureResource, _textureInfos), "textureInfos");
		AddProperty(textureInfos);
	}
}
