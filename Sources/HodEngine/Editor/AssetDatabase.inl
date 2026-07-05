
namespace hod::inline editor
{
	/// @brief
	/// @tparam _Importer_
	/// @return
	template<typename _Importer_, typename... Args>
	bool AssetDatabase::RegisterImporter(Args... extensions)
	{
		// TODO check duplicate
		ImporterEntry* entry = DefaultAllocator::GetInstance().New<ImporterEntry>();
		entry->_factory = []() -> Importer* { return DefaultAllocator::GetInstance().New<_Importer_>(); };
		for (const char* extension : {extensions...})
		{
			entry->_extensions.PushBack(String(extension));
		}

		_importerEntries.PushBack(entry);
		return true;
	}

	template<typename _Cooker_>
	bool AssetDatabase::RegisterCooker(std::string_view assetType)
	{
		// TODO check duplicate
		CookerEntry* entry = DefaultAllocator::GetInstance().New<CookerEntry>();
		entry->_factory = []() -> Cooker* { return DefaultAllocator::GetInstance().New<_Cooker_>(); };
		entry->_assetType = Hash::ComputeXxh3_64(assetType);

		_cookerEntries.PushBack(entry);
		return true;
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
