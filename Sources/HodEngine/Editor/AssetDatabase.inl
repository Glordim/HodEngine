
namespace hod::editor
{
	/// @brief 
	/// @tparam _Importer_ 
	/// @return 
	template<typename _Importer_>
	bool AssetDatabase::RegisterImporter()
	{
		// TODO check duplicate
		_Importer_* importer = new _Importer_();
		_importers.push_back(importer);
		return true;
	}

	/// @brief 
	/// @tparam _Object_ 
	/// @param path 
	/// @return 
	template<typename _Object_>
	std::filesystem::path AssetDatabase::CreateAsset(const std::filesystem::path& path)
	{
		_Object_ object;
		return CreateAsset(&object, _Object_::GetReflectionDescriptor(), path);
	}
}
