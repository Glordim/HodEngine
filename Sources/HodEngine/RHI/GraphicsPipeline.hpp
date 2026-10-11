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
			bool        _useDepth = true;
		};

		GraphicsPipeline();
		virtual ~GraphicsPipeline();

		virtual bool Build(const CreateInfo& createInfo) = 0;

		const std::map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

	protected:
		void MergeSetDescriptors(const Shader& vertexShader, const Shader& fragmentShader);

	protected:
		std::map<uint32_t, ShaderSetDescriptor*> _setDescriptors;
	};
}
