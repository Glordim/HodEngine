#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief
	AssetType::~AssetType() = default;

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
	/// @param registration
	void AssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		(void)registration;
	}

	/// @brief
	void AssetType::OnUnregistered() {}
}
