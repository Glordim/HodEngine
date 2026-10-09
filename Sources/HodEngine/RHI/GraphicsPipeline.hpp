#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstdint>
#include <map>
#include <unordered_map>

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
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

		// bool					link(Shader* vertexShader, Shader* fragmentShader);
		// void					use();

		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& GetSetDescriptors() const;

		bool HasReportedUnsetTexture() const;
		void ReportUnsetTexture(const String& name) const;

	protected:
		std::unordered_map<uint32_t, ShaderSetDescriptor*> _setDescriptors;

	private:
		mutable bool _unsetTextureReported = false; // reported once per pipeline, not once per draw

		// uint32_t				getLocationFromName(const String& name);
	};
}
