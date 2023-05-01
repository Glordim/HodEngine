#pragma once

#include "HodEngine/Renderer/RenderCommand/RenderCommand.h"
#include "HodEngine/Renderer/P2fT2f.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include <vector>

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderCommandMesh : public RenderCommand
		{
		public:

									RenderCommandMesh(const void* vertices, uint32_t vertexCount, uint32_t vertexSize, const uint16_t* indices, uint32_t indexCount, const glm::mat4x4& modelMatrix, MaterialInstance* materialInstance, bool ignoreVisualisationMode = false);
									RenderCommandMesh(const RenderCommandMesh&) = delete;
									RenderCommandMesh(RenderCommandMesh&&) = delete;
									~RenderCommandMesh() override = default;

			void					operator=(const RenderCommandMesh&) = delete;
			void					operator=(RenderCommandMesh&&) = delete;

		public:

			void					Execute(CommandBuffer* commandBuffer) override;

		private:

			std::vector<uint8_t>	_vertices;
			uint32_t				_vertexCount;
			uint32_t				_vertexSize;

			std::vector<uint16_t>	_indices;
			glm::mat4x4				_modelMatrix;
			MaterialInstance*		_materialInstance = nullptr;
			bool					_ignoreVisualisationMode;
		};
	}
}
