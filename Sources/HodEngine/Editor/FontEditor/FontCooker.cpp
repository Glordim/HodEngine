#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Editor/FontEditor/FontCooker.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include <cstdint>

namespace hod::inline editor
{
	bool FontCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)platforms;
		(void)configs;
		(void)languages;

		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* ttfDataBlock = assetContainer.FindDataBlock("ttf");
		if (ttfDataBlock == nullptr)
		{
			return false;
		}
		CompressionStream& ttfInStream = static_cast<CompressionStream&>(*ttfDataBlock->_stream);
		Vector<uint8_t> ttf;
		ttf.Resize(ttfDataBlock->_uncompressedSize);
		if (ttfInStream.Read(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		Stream& ttfOutStream = AddDataBlockStream("ttf", true, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (ttfOutStream.Write(ttf.Data(), ttf.Size()) != ttf.Size())
		{
			return false;
		}

		return true;
	}
}
