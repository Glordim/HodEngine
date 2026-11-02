#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Hash.hpp>

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
	class EditorModuleRegistration;
	class EditorTab;

	/// @brief A type of asset and what the editor does with its assets. Declared once by the editor module adding it (with
	/// HOD_ASSET_TYPE, its name and hash given to all the code using it: TextureAssetType::GetHash()...) and registered by it
	/// (EditorModuleRegistration::RegisterAssetType<_AssetType_>()), by one module only. Each part is optional.
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
		/// registered through registration so that it goes with it. Not its importers: registered by the module once all the types
		/// they import into are
		virtual void OnRegistered(EditorModuleRegistration& registration);
		/// @brief Before its module unloads, its tabs closed already: undoes what OnRegistered did besides registration (which
		/// unregisters on its own)
		virtual void OnUnregistered();
	};
}

/// @brief In an AssetType's declaration: its name, and its hash computed once (xxh3, as the asset files store it)
#define HOD_ASSET_TYPE(NAME)                                          \
public:                                                               \
	static std::string_view GetName()                                 \
	{                                                                 \
		return NAME;                                                  \
	}                                                                 \
                                                                      \
	static uint64_t GetHash()                                         \
	{                                                                 \
		static const uint64_t hash = hod::Hash::ComputeXxh3_64(NAME); \
		return hash;                                                  \
	}                                                                 \
                                                                      \
	std::string_view GetNameV() const override                        \
	{                                                                 \
		return GetName();                                             \
	}                                                                 \
                                                                      \
	uint64_t GetHashV() const override                                \
	{                                                                 \
		return GetHash();                                             \
	}                                                                 \
                                                                      \
private:
