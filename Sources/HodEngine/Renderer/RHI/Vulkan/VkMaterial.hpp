#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Material.hpp"

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API VkMaterial : public Material
	{
	public:

								VkMaterial();
		virtual					~VkMaterial();

		virtual bool			Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

		VkPipeline				GetGraphicsPipeline() const;
		VkPipelineLayout		GetPipelineLayout() const;

	private:
		VkPipeline				_graphicsPipeline;
		VkPipelineLayout		_pipelineLayout;
	};
}
