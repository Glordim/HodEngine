
namespace hod::editor
{
	template<typename Type>
	std::filesystem::path AssetDatabase::CreateAsset(const std::filesystem::path& path)
	{
		FileSystemMapping* childFileSystemMapping = new FileSystemMapping;
		childFileSystemMapping->_path = path;
		childFileSystemMapping->_path.replace_extension(".asset");
		childFileSystemMapping->_path = GenerateUniqueAssetPath(childFileSystemMapping->_path);
		//childFileSystemMapping->_lastWriteTime = 0;
		childFileSystemMapping->_parentFolder = FindFileSystemMappingFromPath(path.parent_path());

		std::shared_ptr<Asset> asset = std::make_shared<Type>(childFileSystemMapping->_path);
		childFileSystemMapping->_asset = asset;
		childFileSystemMapping->_type = FileSystemMapping::Type::AssetType;
		_uidToAssetMap.emplace(asset->GetUid(), asset);

		childFileSystemMapping->_parentFolder->_childrenAsset.push_back(childFileSystemMapping);

		asset->Save();

		return childFileSystemMapping->_path;
	}
}
