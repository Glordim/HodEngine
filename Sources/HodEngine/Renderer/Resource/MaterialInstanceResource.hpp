#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Resource/Resource.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"

#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::renderer
{
	class MaterialInstance;
	
	class HOD_RENDERER_API MaterialInstanceResource : public Resource
	{
		REFLECTED_CLASS(MaterialInstanceResource, Resource)

	public:

									MaterialInstanceResource() = default;
									MaterialInstanceResource(const MaterialInstanceResource&) = delete;
									MaterialInstanceResource(MaterialInstanceResource&&) = delete;
									~MaterialInstanceResource() override;

		MaterialInstanceResource&	operator = (const MaterialInstanceResource&) = delete;
		MaterialInstanceResource&	operator = (MaterialInstanceResource&&) = delete;

	public:

		bool						Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		MaterialInstance*			GetMaterialInstance() const;

		void						ApplyInitialParams(ReflectionDescriptor* reflectionDescriptor, void* instance, std::string path);

	private:

		MaterialInstance*				_materialInstance = nullptr;

		WeakResource<MaterialResource>	_material;
		Document						_params;

		std::vector<WeakResource<TextureResource>> _texturesRetain;
	};
}