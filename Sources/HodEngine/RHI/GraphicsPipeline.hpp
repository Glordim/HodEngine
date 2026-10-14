#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstdint>
#include <map>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/RHI/Enums.hpp"

namespace hod::inline rhi
{
	class Shader;
	class Texture;
	class VertexInput;
	class ShaderSetDescriptor;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API GraphicsPipeline
	{
	public:
		enum PolygonMode
		{
			Fill,
			Line,
			Point,
		};

		REFLECTED_ENUM(HOD_RHI_API, PolygonMode);

		enum Topololy
		{
			POINT,
			LINE,
			LINE_STRIP,
			TRIANGLE,
			TRIANGLE_FAN,
		};

		REFLECTED_ENUM(HOD_RHI_API, Topololy);

		/// @brief What a pipeline is built from, see RhiDevice::CreateGraphicsPipeline
		struct CreateInfo
		{
			// One per input of the vertex shader, in the order it declares them
			const VertexInput* _vertexInputs = nullptr;
			uint32_t           _vertexInputCount = 0;

			Shader* _vertexShader = nullptr;
			Shader* _fragmentShader = nullptr;

			PolygonMode _polygonMode = PolygonMode::Fill;
			Topololy    _topololy = Topololy::TRIANGLE;
			BlendMode   _blendMode = BlendMode::Alpha;
			CullMode    _cullMode = CullMode::None;

			// Against the depth buffer of what is drawn into: drawn only where it is nearer than what is already there (_depthTest),
			// and becomes the depth there (_depthWrite). Each works without the other.
			bool _depthTest = false;
			bool _depthWrite = false;

			// What the render passes it is drawn in draw into: it can only be bound in a render pass of these formats.
			// PixelFormat::None for no attachment of that kind. The defaults are the formats of every presentation surface.
			PixelFormat _colorFormat = PixelFormat::R8G8B8A8_UNorm;
			PixelFormat _depthFormat = PixelFormat::D16_UNorm;
		};

		GraphicsPipeline();
		virtual ~GraphicsPipeline();

		virtual bool Build(const CreateInfo& createInfo) = 0;

		const std::map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

		PixelFormat GetColorFormat() const;
		PixelFormat GetDepthFormat() const;

	protected:
		// To be called first by Build: keeps the formats of the CreateInfo, false if they are not ones of attachments,
		// or if its states need an attachment it has no format for
		bool SetTargetFormats(const CreateInfo& createInfo);
		void MergeSetDescriptors(const Shader& vertexShader, const Shader& fragmentShader);

	protected:
		std::map<uint32_t, ShaderSetDescriptor*> _setDescriptors;

	private:
		PixelFormat _colorFormat = PixelFormat::None;
		PixelFormat _depthFormat = PixelFormat::None;
	};
}
