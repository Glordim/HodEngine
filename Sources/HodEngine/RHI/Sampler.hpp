#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Enums.hpp"

namespace hod::inline rhi
{
	/// @brief How a texture is read: filtering and addressing.
	/// Samplers are shared and owned by the RhiDevice, see RhiDevice::GetSampler.
	class HOD_RHI_API Sampler
	{
	public:
		struct CreateInfo
		{
			FilterMode _filterMode = FilterMode::Linear;
			WrapMode   _wrapMode = WrapMode::Clamp;

			bool operator==(const CreateInfo& other) const = default;
		};

	public:
		Sampler(const Sampler&) = delete;
		Sampler(Sampler&&) = delete;
		virtual ~Sampler();

		Sampler& operator=(const Sampler&) = delete;
		Sampler& operator=(Sampler&&) = delete;

		const CreateInfo& GetCreateInfo() const;

	protected:
		Sampler(const CreateInfo& createInfo);

	private:
		CreateInfo _createInfo;
	};
}
