#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/AudioEditor/AudioImporter.hpp"

#include "HodEngine/Audio/AudioResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(AudioImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	AudioImporter::AudioImporter()
	{
		SetSupportedDataFileExtensions("wav");
	}

	/// @brief
	/// @param path
	/// @return
	bool AudioImporter::WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail,
	                                    ImporterSettings& /*settings*/)
	{
		// TODO
		(void)meta;
		(void)thumbnail;

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
		if (data.Read(&header, sizeof(WAVHeader)) != sizeof(WAVHeader))
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
			data.Seek(header.dataSize, Stream::SeekOrigin::Current);
			if (data.Read(&header.dataMarker, sizeof(char) * 4 + sizeof(uint32_t)) != sizeof(char) * 4 + sizeof(uint32_t))
			{
				OUTPUT_ERROR("AudioImporter : Can't read data section");
				return false;
			}
		}

		Vector<uint8_t> audioBuffer;
		audioBuffer.Resize(header.dataSize);
		if (data.Read(audioBuffer.Data(), header.dataSize) != header.dataSize)
		{
			OUTPUT_ERROR("AudioImporter : Can't read data");
			return false;
		}

		//AudioImporterSettings& audioSettings = (AudioImporterSettings&)settings;

		AudioResource audioResource;
		audioResource._channelCount = header.numChannels;
		audioResource._sampleRate = header.sampleRate;

		if (Serializer::Serialize(audioResource, document.GetRootNode()) == false)
		{
			// TODO message
			return false;
		}

		uint32_t sampleCount = header.dataSize / (header.bitsPerSample / 8);
		Vector<float> floatSamples;
		floatSamples.Resize(sampleCount);

		if (header.bitsPerSample == 8)
		{
			for (uint32_t i = 0; i < sampleCount; ++i)
				floatSamples[i] = (static_cast<float>(audioBuffer[i]) - 128.0f) / 128.0f;
		}
		else if (header.bitsPerSample == 16)
		{
			const int16_t* src = reinterpret_cast<const int16_t*>(audioBuffer.Data());
			for (uint32_t i = 0; i < sampleCount; ++i)
				floatSamples[i] = static_cast<float>(src[i]) / 32768.0f;
		}
		else if (header.bitsPerSample == 24)
		{
			for (uint32_t i = 0; i < sampleCount; ++i)
			{
				int32_t sample = (audioBuffer[i * 3 + 2] << 16) | (audioBuffer[i * 3 + 1] << 8) | audioBuffer[i * 3];
				if (sample & 0x800000)
					sample |= 0xFF000000;
				floatSamples[i] = static_cast<float>(sample) / 8388608.0f;
			}
		}
		else if (header.bitsPerSample == 32)
		{
			const int32_t* src = reinterpret_cast<const int32_t*>(audioBuffer.Data());
			for (uint32_t i = 0; i < sampleCount; ++i)
				floatSamples[i] = static_cast<float>(src[i]) / 2147483648.0f;
		}
		else
		{
			OUTPUT_ERROR("AudioImporter : unsupported bitsPerSample (%u)", header.bitsPerSample);
			return false;
		}

		Resource::Data sampleData;
		sampleData._buffer = DefaultAllocator::GetInstance().Allocate(sampleCount * sizeof(float));
		std::memcpy(sampleData._buffer, floatSamples.Data(), sampleCount * sizeof(float));
		sampleData._size = sampleCount * sizeof(float);
		datas.push_back(sampleData);

		return true;
	}

	// TODO Move all virtual in Ctor const init ?

	/// @brief
	/// @return
	const char* AudioImporter::GetTypeName() const
	{
		return "AudioImporter";
	}

	ReflectionDescriptor* AudioImporter::GetResourceDescriptor() const
	{
		return &AudioResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> AudioImporter::AllocateSettings() const
	{
		return std::make_shared<AudioImporterSettings>();
	}
}
