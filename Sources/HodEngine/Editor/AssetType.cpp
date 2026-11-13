#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief
	AssetType::~AssetType() = default;

	/// @brief
	/// @param baseName
	/// @param extension
	/// @return
	String AssetType::MakeFileName(std::string_view baseName, std::string_view extension)
	{
		String fileName;
		fileName.Reserve(baseName.size() + 1 + extension.size());
		fileName = baseName;
		fileName += ".";
		fileName += extension;
		return fileName;
	}

	/// @brief
	/// @return
	Cooker* AssetType::CreateCooker() const
	{
		return nullptr;
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* AssetType::GetCookedResource() const
	{
		return nullptr;
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* AssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		(void)asset;
		return nullptr;
	}

	/// @brief
	/// @return
	Texture* AssetType::GetIcon() const
	{
		return nullptr;
	}

	/// @brief
	/// @param asset
	void AssetType::OnRenamed(Asset& asset) const
	{
		(void)asset;
	}

	/// @brief
	/// @param editorRegistrar
	void AssetType::OnRegistered(EditorRegistrar& editorRegistrar)
	{
		(void)editorRegistrar;
	}

	/// @brief
	void AssetType::OnUnregistered() {}
}
