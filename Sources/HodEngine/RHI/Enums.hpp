#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline rhi
{
	enum class FilterMode : uint8_t
	{
		Nearest,
		Linear,

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, FilterMode);

	enum class WrapMode : uint8_t
	{
		Clamp,
		Repeat,

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, WrapMode);

	/// @brief How what is drawn is mixed with what the render target already holds
	enum class BlendMode : uint8_t
	{
		Opaque,             // replaces it
		Alpha,              // source * alpha + destination * (1 - alpha)
		PremultipliedAlpha, // source + destination * (1 - alpha), for colors already multiplied by their alpha
		Additive,           // source * alpha + destination

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, BlendMode);

	/// @brief The triangles that are not drawn, by the face they show.
	/// The front face of a triangle is the one its vertices are counter-clockwise on, +y going up.
	enum class CullMode : uint8_t
	{
		None,
		Back,
		Front,

		Count,
	};
	REFLECTED_ENUM2(HOD_RHI_API, CullMode);

	/// @brief Format of what a render pass draws into, see RenderTarget::Init and GraphicsPipeline::CreateInfo.
	/// The depth is 16 bits: always supported, and linear depth (orthographic projections) needs no more. No stencil, nothing uses it.
	enum class PixelFormat : uint8_t
	{
		None, // no attachment of that kind
		R8G8B8A8_UNorm,
		D16_UNorm,

		Count,
	};

}
