#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/AudioResource.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>
#include <cstring>

namespace hod::inline audio
{
	DESCRIBE_REFLECTED_CLASS(AudioResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AudioResource::_sampleRate, "_sampleRate");
		AddPropertyT(reflectionDescriptor, &AudioResource::_channelCount, "_channelCount");
	}

	/// @brief
	/// @return
	AudioResource::~AudioResource()
	{
	}

	/// @brief
	/// @param document
	/// @param stream
	/// @return
	bool AudioResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* infoDataBlock = resourceContainer.FindDataBlock("Info");
		if (infoDataBlock == nullptr)
		{
			OUTPUT_ERROR("AudioResource::Initialize: missing 'Info' data block");
			return false;
		}

		uint8_t channelCount = 0;
		uint8_t bitsPerSample = 0;
		infoDataBlock->_stream->Read(&channelCount, sizeof(channelCount));
		infoDataBlock->_stream->Read(&bitsPerSample, sizeof(bitsPerSample));
		infoDataBlock->_stream->Read(&_sampleRate, sizeof(_sampleRate));

		_channelCount = channelCount;
		if (_channelCount == 0)
		{
			OUTPUT_ERROR("AudioResource::Initialize: invalid channel count");
			return false;
		}

		const ResourceContainer::DataBlockInfo* pcmDataBlock = resourceContainer.FindDataBlock("Pcm");
		if (pcmDataBlock == nullptr)
		{
			OUTPUT_ERROR("AudioResource::Initialize: missing 'Pcm' data block");
			return false;
		}

		_samples.Resize(pcmDataBlock->_uncompressedSize / sizeof(float));
		if (pcmDataBlock->_stream->Read(_samples.Data(), pcmDataBlock->_uncompressedSize) != pcmDataBlock->_uncompressedSize)
		{
			OUTPUT_ERROR("AudioResource::Initialize: can't read 'Pcm' data block");
			return false;
		}

		_frameCount = _samples.Size() / _channelCount;

		return true;
	}
}
