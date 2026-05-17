#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/Importer/AudioImporter.hpp"

#include "HodEngine/Audio/AudioResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <cstdint>
#include <sstream>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(AudioImporterSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AudioImporterSettings::_loop, "Loop");
	}

	/// @brief
	AudioImporter::AudioImporter()
	{
		SetSupportedDataFileExtensions("wav");
	}

	/// @brief
	/// @param path
	/// @return
	bool AudioImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail,
	                                    ImporterSettings& settings)
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
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(&header), sizeof(WAVHeader)) != (int32_t)sizeof(WAVHeader))
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
			FileSystem::GetInstance()->Seek(data, header.dataSize, FileSystem::SeekMode::Current);
			if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(&header.dataMarker), sizeof(char) * 4 + sizeof(uint32_t)) != (int32_t)(sizeof(char) * 4 + sizeof(uint32_t)))
			{
				OUTPUT_ERROR("AudioImporter : Can't read data section");
				return false;
			}
		}

		Vector<uint8_t> audioBuffer;
		audioBuffer.Resize(header.dataSize);
		if (FileSystem::GetInstance()->Read(data, reinterpret_cast<char*>(audioBuffer.Data()), header.dataSize) != (int32_t)header.dataSize)
		{
			OUTPUT_ERROR("AudioImporter : Can't read data");
			return false;
		}

		AudioImporterSettings& audioSettings = (AudioImporterSettings&)settings;

		AudioResource audioResource;
		audioResource._loop = audioSettings._loop;

		if (Serializer::Serialize(audioResource, document.GetRootNode()) == false)
		{
			// TODO message
			return false;
		}

		Resource::Data sampleData;
		sampleData._buffer = DefaultAllocator::GetInstance().Allocate(audioBuffer.Size());
		std::memcpy(sampleData._buffer, audioBuffer.Data(), audioBuffer.Size());
		sampleData._size = audioBuffer.Size();
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
