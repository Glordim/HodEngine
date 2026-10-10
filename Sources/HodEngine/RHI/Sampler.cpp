#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Sampler.hpp"

namespace hod::inline rhi
{
	/// @brief
	/// @param createInfo
	Sampler::Sampler(const CreateInfo& createInfo)
	: _createInfo(createInfo)
	{
	}

	/// @brief
	Sampler::~Sampler() {}

	/// @brief
	/// @return
	const Sampler::CreateInfo& Sampler::GetCreateInfo() const
	{
		return _createInfo;
	}
}
