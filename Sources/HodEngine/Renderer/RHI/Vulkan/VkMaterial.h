#pragma once

#include "HodEngine/Renderer/RHI/Material.h"

#include <vulkan/vulkan.h>

#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSetLayout.h"

#include <map>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkMaterial : public Material
		{
		public:

														VkMaterial();
			virtual										~VkMaterial();

			virtual bool								Build(VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

			VkPipeline									GetGraphicsPipeline() const;
			VkPipelineLayout							GetPipelineLayout() const;

			const std::map<int, DescriptorSetLayout>&	GetDescriptorSetLayoutMap() const;

		private:
			VkPipeline									_graphicsPipeline;
			VkPipelineLayout							_pipelineLayout;

			std::map<int, DescriptorSetLayout>			_descriptorSetLayoutMap;
		};
	}
}