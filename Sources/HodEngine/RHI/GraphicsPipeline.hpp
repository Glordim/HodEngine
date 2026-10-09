#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstdint>
#include <map>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

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

		GraphicsPipeline();
		virtual ~GraphicsPipeline();

		virtual bool Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
							PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) = 0;

		const std::map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

		bool HasReportedUnsetTexture() const;
		void ReportUnsetTexture(const String& name) const;

	protected:
		void MergeSetDescriptors(const Shader& vertexShader, const Shader& fragmentShader);

	protected:
		std::map<uint32_t, ShaderSetDescriptor*> _setDescriptors;

	private:
		mutable bool _unsetTextureReported = false; // reported once per pipeline, not once per draw
	};
}
