
namespace hod::editor
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

	/// @brief 
	/// @tparam _Object_ 
	/// @param path 
	/// @return 
	template<typename _Object_, typename _Importer_>
	std::filesystem::path AssetDatabase::CreateAsset(const std::filesystem::path& path)
	{
		_Object_ object;
		_Importer_ importer;
		return CreateAsset(&object, &_Object_::GetReflectionDescriptor(), importer.AllocateSettings(), importer.GetTypeName(), path);
	}
}
