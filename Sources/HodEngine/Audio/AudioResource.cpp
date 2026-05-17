#include "HodEngine/Audio/Pch.hpp"
#include "HodEngine/Audio/AudioResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline audio
{
	DESCRIBE_REFLECTED_CLASS(AudioResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AudioResource::_loop, "_loop");
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
	bool AudioResource::Initialize(const DocumentNode& documentNode, const Vector<Resource::Data>& datas)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			OUTPUT_ERROR("AudioResource::Initialize: unable to deserialize");
			return false;
		}

		if (datas.Empty())
		{
			OUTPUT_ERROR("AudioResource::Initialize: invalid data count");
			return false;
		}

		//const Resource::Data& data = datas[0];

		return true;
	}
}
