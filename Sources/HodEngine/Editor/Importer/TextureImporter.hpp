#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::editor
{
	enum class MeshType
	{
		Rect,
		Tight,
	};

	struct HOD_API SpriteData
	{
		REFLECTED_CLASS_NO_PARENT(SpriteData)

	public:

		virtual ~SpriteData() = default;

		UID			_uid;
		Rect		_rect;
		MeshType	_meshType = MeshType::Rect;
	};

	class HOD_API TextureImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(TextureImporterSettings, ImporterSettings)

	public:

		bool									_generateMipmap = false;
		renderer::FilterMode					_filterMode = renderer::FilterMode::Linear;
		renderer::WrapMode						_wrapMode = renderer::WrapMode::Clamp;
		std::vector<SpriteData> 				_spriteDatas;
	};

	class HOD_API TextureImporter : public Importer
	{
	public:

							TextureImporter();
							TextureImporter(const TextureImporter&) = delete;
							TextureImporter(TextureImporter&&) = delete;
							~TextureImporter() override = default;

		TextureImporter&	operator = (const TextureImporter&) = delete;
		TextureImporter&	operator = (TextureImporter&&) = delete;

	public:

		ImporterSettings*		AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
