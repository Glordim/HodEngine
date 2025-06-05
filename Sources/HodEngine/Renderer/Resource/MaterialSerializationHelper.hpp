#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include <HodEngine/Core/Resource/WeakResource.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::renderer
{
	class Material;
	class MaterialInstance;
	class TextureResource;
	
	struct ShaderParameter
	{
		enum class Type
		{
			Unknown,

			Bool,
			Int,
			Float,
			Float2,
			Float4,
			Texture,
		};

		std::string	_name;
		Type		_type = Type::Unknown;
	};

	class HOD_RENDERER_API MaterialSerializationHelper
	{
	public:
		static void GenerateParameters(const Material& material, Vector<ShaderParameter>& params);

		static void ApplyParamsFromDocument(MaterialInstance& materialInstance, const Document::Node& paramsNode, Vector<WeakResource<TextureResource>>& textureResources);
		static void ApplyReflectedParams(MaterialInstance& materialInstance, const ReflectionDescriptor& reflectionDescriptor, void* instance, String path, Vector<WeakResource<TextureResource>>& textureResources);
	};
}
