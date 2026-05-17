#pragma once
#include "HodEngine/Audio/Export.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"

namespace hod::inline audio
{
	class HOD_AUDIO_API AudioResource : public Resource
	{
		REFLECTED_CLASS(AudioResource, Resource)

	public:

							AudioResource() = default;
							AudioResource(const AudioResource&) = delete;
							AudioResource(AudioResource&&) = delete;
							~AudioResource() override;

		AudioResource&	operator = (const AudioResource&) = delete;
		AudioResource&	operator = (AudioResource&&) = delete;

	public:

		bool				Initialize(const DocumentNode& documentNode, const Vector<Resource::Data>& datas) override;

	public:

		bool					_loop = false;
	};
}