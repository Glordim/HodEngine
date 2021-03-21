#pragma once

#include "RenderCommand.h"
#include "../P2fT2f.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include <vector>

namespace HOD
{
	namespace RENDERER
	{
		class MaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderCommandMesh : public RenderCommand
		{
		public:

									RenderCommandMesh(const P2fT2f* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const glm::mat4x4& modelMatrix, MaterialInstance* materialInstance);
									RenderCommandMesh(const RenderCommandMesh&) = delete;
									RenderCommandMesh(RenderCommandMesh&&) = delete;
									~RenderCommandMesh() override = default;

			void					operator=(const RenderCommandMesh&) = delete;
			void					operator=(RenderCommandMesh&&) = delete;

		public:

			void					Execute(CommandBuffer* commandBuffer) override;

		private:

			std::vector<P2fT2f>		_vertices;
			std::vector<uint16_t>	_indices;
			glm::mat4x4				_modelMatrix;
			MaterialInstance*		_materialInstance = nullptr;
		};
	}
}
