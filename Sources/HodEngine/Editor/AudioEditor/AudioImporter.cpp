#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/AudioEditor/AudioImporter.hpp"

#include <cstdint>

namespace hod::inline editor
{
	AudioImporter::AudioImporter()
	{
		SetAssetExtension("audio");
		SetAssetType("Audio");
	}

	/// @brief
	/// @param path
	/// @return
	bool AudioImporter::FillDataBlock(Stream& source, ImporterSettings* /*importSettings*/)
	{
		#pragma pack(push, 1)
		struct WAVHeader
		{
			char     riffType[4];
			uint32_t fileSize;
			char     waveMarker[4];

			char     fmtMarker[4];
			uint32_t fmtLength;
			uint16_t audioFormat;
			uint16_t numChannels;
			uint32_t sampleRate;
			uint32_t byteRate;
			uint16_t blockAlign;
			uint16_t bitsPerSample;

			char     dataMarker[4];
			uint32_t dataSize;
		};
		#pragma pack(pop)

		WAVHeader header;
		if (source.Read(&header, sizeof(WAVHeader)) != sizeof(WAVHeader))
		{
			OUTPUT_ERROR("AudioImporter : Can't read Wav header");
			return false;
		}
		if (std::string_view(header.riffType, 4) != "RIFF" || std::string_view(header.waveMarker, 4) != "WAVE" || std::string_view(header.fmtMarker, 4) != "fmt ")
		{
			OUTPUT_ERROR("AudioImporter : invalid Wav header");
			return false;
		}
		if (header.audioFormat != 1)
		{
			OUTPUT_ERROR("AudioImporter : only pcm Wav supported");
			return false;
		}

		while (std::string_view(header.dataMarker, 4) != "data")
		{
			source.Seek(header.dataSize, Stream::SeekOrigin::Current);
			if (source.Read(&header.dataMarker, sizeof(char) * 4 + sizeof(uint32_t)) != sizeof(char) * 4 + sizeof(uint32_t))
			{
				OUTPUT_ERROR("AudioImporter : Can't read data section");
				return false;
			}
		}

		uint8_t channelCount = header.numChannels;
		uint8_t bitsPerSample = header.bitsPerSample;
		uint32_t sampleRate = header.sampleRate;
		uint32_t sampleCount = header.dataSize / (header.bitsPerSample / 8) / channelCount;

		Stream& infoStream = AddDataBlockStream("Info", false);
		infoStream.Write(&channelCount, sizeof(channelCount));
		infoStream.Write(&bitsPerSample, sizeof(bitsPerSample));
		infoStream.Write(&sampleRate, sizeof(sampleRate));
		infoStream.Write(&sampleCount, sizeof(sampleCount));

		if (header.dataSize != sampleCount * (header.bitsPerSample / 8) * channelCount)
		{
			OUTPUT_ERROR("AudioImporter : dataSize mismatch sampleCount * bitPerSample * channelCount");
			return false;
		}

		Vector<uint8_t> pcm;
		pcm.Resize(header.dataSize);
		if (source.Read(pcm.Data(), header.dataSize) != header.dataSize)
		{
			OUTPUT_ERROR("AudioImporter : Can't read data");
			return false;
		}
		Stream& pcmStream = AddDataBlockStream("Pcm", true);
		pcmStream.Write(pcm.Data(), sampleCount * (header.bitsPerSample / 8) * channelCount);

		return true;
	}
}
