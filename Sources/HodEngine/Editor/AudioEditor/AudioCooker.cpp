#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/AudioEditor/AudioCooker.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/GameSystems/Resource/ResourceVariant.hpp"
#include <cstdint>
#include <utility>

namespace hod::inline editor
{
	AudioCooker::AudioCooker()
	{
		SetCookerVersion(1);
	}

	/// @brief
	/// @param path
	/// @return
	bool AudioCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)platforms;
		(void)configs;
		(void)languages;

		AssetContainer assetContainer;
		if (assetContainer.Load(asset.GetPath()) == false)
		{
			return false;
		}

		const AssetContainer::DataBlockInfo* infoDataBlock = assetContainer.FindDataBlock("Info");
		if (infoDataBlock == nullptr)
		{
			return false;
		}
		Stream& infoInStream = *infoDataBlock->_stream;

		uint8_t channelCount = 0;
		uint8_t bitsPerSample = 0;
		uint32_t sampleRate = 0;
		uint32_t sampleCount = 0;
		infoInStream.Read(&channelCount, sizeof(channelCount));
		infoInStream.Read(&bitsPerSample, sizeof(bitsPerSample));
		infoInStream.Read(&sampleRate, sizeof(sampleRate));
		infoInStream.Read(&sampleCount, sizeof(sampleCount));

		Stream& infoOutStream = AddDataBlockStream("Info", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		infoOutStream.Write(&channelCount, sizeof(channelCount));
		infoOutStream.Write(&bitsPerSample, sizeof(bitsPerSample));
		infoOutStream.Write(&sampleRate, sizeof(sampleRate));
		infoOutStream.Write(&sampleCount, sizeof(sampleCount));

		const AssetContainer::DataBlockInfo* pcmDataBlock = assetContainer.FindDataBlock("Pcm");
		if (pcmDataBlock == nullptr)
		{
			return false;
		}
		Stream& pcmInStream = *pcmDataBlock->_stream;

		Vector<uint8_t> pcm;
		pcm.Resize(sampleCount * (bitsPerSample / 8) * channelCount);
		if (pcmInStream.Read(pcm.Data(), pcm.Size()) != pcm.Size())
		{
			OUTPUT_ERROR("AudioImporter : Can't read data");
			return false;
		}

		Vector<float> floatSamples;
		floatSamples.Resize(sampleCount * channelCount);

		if (bitsPerSample == 8)
		{
			for (uint32_t i = 0; i < sampleCount * channelCount; ++i)
				floatSamples[i] = (static_cast<float>(pcm[i]) - 128.0f) / 128.0f;
		}
		else if (bitsPerSample == 16)
		{
			const int16_t* src = reinterpret_cast<const int16_t*>(pcm.Data());
			for (uint32_t i = 0; i < sampleCount * channelCount; ++i)
				floatSamples[i] = static_cast<float>(src[i]) / 32768.0f;
		}
		else if (bitsPerSample == 24)
		{
			for (uint32_t i = 0; i < sampleCount * channelCount; ++i)
			{
				int32_t sample = (pcm[i * 3 + 2] << 16) | (pcm[i * 3 + 1] << 8) | pcm[i * 3];
				if (sample & 0x800000)
					sample |= 0xFF000000;
				floatSamples[i] = static_cast<float>(sample) / 8388608.0f;
			}
		}
		else if (bitsPerSample == 32)
		{
			const int32_t* src = reinterpret_cast<const int32_t*>(pcm.Data());
			for (uint32_t i = 0; i < sampleCount * channelCount; ++i)
				floatSamples[i] = static_cast<float>(src[i]) / 2147483648.0f;
		}
		else
		{
			OUTPUT_ERROR("AudioImporter : unsupported bitsPerSample {}", bitsPerSample);
			return false;
		}

		Stream& pcmOutStream = AddDataBlockStream("Pcm", false, std::to_underlying(Platform::All), std::to_underlying(Config::All), std::to_underlying(ResourceVariant::Language::All));
		if (pcmOutStream.Write(floatSamples.Data(), sampleCount * sizeof(float) * channelCount) != sampleCount * sizeof(float) * channelCount)
		{
			OUTPUT_ERROR("AudioImporter : unable to write pcm");
			return false;
		}

		return true;
	}
}
