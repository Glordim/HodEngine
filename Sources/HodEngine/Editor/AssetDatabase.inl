#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @tparam _Importer_
	/// @return
	template<typename _Importer_, typename... Args>
	bool AssetDatabase::RegisterImporter(Args... extensions)
	{
		for (const char* extension : {extensions...})
		{
			if (HasImporter(extension))
			{
				OUTPUT_ERROR("AssetDatabase::RegisterImporter: the extension {} has an importer already", extension);
				return false;
			}
		}

		// The asset type it imports into is known from an instance only (see Importer::SetAssetType)
		_Importer_* importer = DefaultAllocator::GetInstance().New<_Importer_>();
		if (FindAssetType(importer->GetAssetType()) == nullptr)
		{
			OUTPUT_WARNING("AssetDatabase::RegisterImporter: the importer of the .{} assets imports into an asset type not registered (register it first)",
			               importer->GetAssetExtension());
		}
		DefaultAllocator::GetInstance().Delete(importer);

		ImporterEntry* entry = DefaultAllocator::GetInstance().New<ImporterEntry>();
		entry->_factory = &NewImporter<_Importer_>;
		for (const char* extension : {extensions...})
		{
			entry->_extensions.PushBack(String(extension));
		}

		_importerEntries.PushBack(entry);
		return true;
	}

	template<typename _Importer_>
	void AssetDatabase::UnregisterImporter()
	{
		UnregisterImporter(&NewImporter<_Importer_>);
	}

	template<typename _Importer_>
	Importer* AssetDatabase::NewImporter()
	{
		return DefaultAllocator::GetInstance().New<_Importer_>();
	}

	/// @brief
	/// @tparam _Object_
	/// @param path
	/// @return
	template<typename _Object_, typename _Importer_>
	Path AssetDatabase::CreateAsset(const Path& path)
	{
		_Object_   object;
		_Importer_ importer;
		return CreateAsset(&object, &_Object_::GetReflectionDescriptor(), importer.AllocateSettings(), importer.GetTypeName(), path);
	}
}
