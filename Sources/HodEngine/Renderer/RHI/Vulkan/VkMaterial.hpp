#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Material.hpp"

#include <vulkan/vulkan.h>

#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSetLayout.hpp"

#include <map>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API VkMaterial : public Material
		{
		public:

														VkMaterial();
			virtual										~VkMaterial();

			virtual bool								Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

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
