#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/String.hpp>

#include <cstdint>
#include <memory>
#include <string_view>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline rhi
{
	class Texture;
}

namespace hod::inline editor
{
	class Asset;
	class Cooker;
	class EditorRegistrar;
	class EditorTab;

	/// @brief A type of asset and what the editor does with its assets. Declared once by the editor module adding it (with
	/// HOD_ASSET_TYPE, its name, hash and extension given to all the code using it: TextureAssetType::GetHash(),
	/// TextureAssetType::GetFileName("Goblin")...) and registered by it
	/// (EditorRegistrar::RegisterAssetType<_AssetType_>()), by one module only. Each part is optional.
	class HOD_EDITOR_API AssetType
	{
	public:
		AssetType() = default;
		AssetType(const AssetType&) = delete;
		AssetType(AssetType&&) = delete;
		virtual ~AssetType();

		AssetType& operator=(const AssetType&) = delete;
		AssetType& operator=(AssetType&&) = delete;

	public:
		virtual std::string_view GetNameV() const = 0;
		/// @brief Of the name: what the asset files store (see AssetContainer::GetAssetType())
		virtual uint64_t GetHashV() const = 0;
		/// @brief Of its asset files, without the dot ("texture"), its own (see AssetDatabase::RegisterAssetType). Only for the
		/// users and their tools (version control attributes, searches...): the editor knows an asset by its header, not by it
		virtual std::string_view GetExtensionV() const = 0;

		/// @brief baseName + '.' + extension
		static String MakeFileName(std::string_view baseName, std::string_view extension);

		/// @brief A new instance per call, usable concurrently with the other ones (see AssetDatabase::AcquireCooker)
		virtual Cooker* CreateCooker() const;
		/// @brief The resource its assets cook into (see AssetDatabase::ListAsset)
		virtual const ReflectionDescriptor* GetCookedResource() const;
		/// @brief The tab opening one of its assets (see Editor::OpenAsset)
		virtual EditorTab* CreateEditorTab(std::shared_ptr<Asset> asset) const;
		/// @brief Of its assets without a thumbnail in the asset browser
		virtual Texture* GetIcon() const;

		/// @brief Called on one of its assets given another name (not only moved to another folder), by AssetDatabase::Move or on
		/// disk, once its path is the new one: for the types holding their name (a UI prefab's root...)
		virtual void OnRenamed(Asset& asset) const;

		/// @brief Once registered by its module (not when refused): what it adds along with it (its asset browser actions...),
		/// registered through editorRegistrar so that it goes with it. Not its importers: registered by the module once all the types
		/// they import into are
		virtual void OnRegistered(EditorRegistrar& editorRegistrar);
		/// @brief Before its module unloads, its tabs closed already: undoes what OnRegistered did besides registration (which
		/// unregisters on its own)
		virtual void OnUnregistered();
	};
}

/// @brief In an AssetType's declaration: its name, its hash computed once (xxh3, as the asset files store it), and the extension of
/// its asset files (without the dot, lowercase: "texture")
#define HOD_ASSET_TYPE(NAME, EXTENSION)                                         \
public:                                                                         \
	static std::string_view GetName()                                           \
	{                                                                           \
		return NAME;                                                            \
	}                                                                           \
                                                                                \
	static uint64_t GetHash()                                                   \
	{                                                                           \
		static const uint64_t hash = hod::Hash::ComputeXxh3_64(NAME);           \
		return hash;                                                            \
	}                                                                           \
                                                                                \
	static std::string_view GetExtension()                                      \
	{                                                                           \
		return EXTENSION;                                                       \
	}                                                                           \
                                                                                \
	/** @brief The file name of one of its assets: baseName.extension */        \
	static hod::String GetFileName(std::string_view baseName)                   \
	{                                                                           \
		return hod::editor::AssetType::MakeFileName(baseName, GetExtension()); \
	}                                                                           \
                                                                                \
	std::string_view GetNameV() const override                                  \
	{                                                                           \
		return GetName();                                                       \
	}                                                                           \
                                                                                \
	uint64_t GetHashV() const override                                          \
	{                                                                           \
		return GetHash();                                                       \
	}                                                                           \
                                                                                \
	std::string_view GetExtensionV() const override                             \
	{                                                                           \
		return GetExtension();                                                  \
	}                                                                           \
                                                                                \
private:
