
namespace hod::inline editor
{
	/// @brief
	/// @tparam _Importer_
	/// @return
	template<typename _Importer_>
	bool AssetDatabase::RegisterImporter()
	{
		// TODO check duplicate
		_Importer_* importer = DefaultAllocator::GetInstance().New<_Importer_>();
		_importers.push_back(importer);
		return true;
	}

	template<typename _Cooker_>
	bool AssetDatabase::RegisterCooker()
	{
		// TODO check duplicate
		_Cooker_* cooker = DefaultAllocator::GetInstance().New<_Cooker_>();
		_cookers.push_back(cooker);
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
