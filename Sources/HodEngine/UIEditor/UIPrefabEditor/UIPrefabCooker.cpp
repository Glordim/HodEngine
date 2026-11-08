#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabCooker.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

namespace hod::inline editor
{
	bool UIPrefabCooker::FillDataBlock(const Asset& asset, uint32_t /*platforms*/, uint8_t /*configs*/, uint32_t /*languages*/)
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* nodesDataBlock = assetContainer.FindDataBlock("Nodes");
		if (nodesDataBlock == nullptr)
		{
			return false;
		}

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, *nodesDataBlock->_stream) == false)
		{
			return false;
		}
		// A prefab renamed outside of the editor, or saved before its root was named after it, isn't renamed in its file yet
		UIPrefabEditorTab::NamePrefabRootAfterAsset(document, asset.GetName());

		Stream& nodesStream = AddDataBlockStream("Nodes", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, nodesStream) == false)
		{
			return false;
		}
		return true;
	}
}
