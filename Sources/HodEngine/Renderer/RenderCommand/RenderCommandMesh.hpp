#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/P2fT2f.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

#include <vector>

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API RenderCommandMesh : public RenderCommand
		{
		public:

									RenderCommandMesh(const void* vertices, uint32_t vertexCount, uint32_t vertexSize, const uint16_t* indices, uint32_t indexCount, const Matrix4& modelMatrix, const MaterialInstance* materialInstance, bool ignoreVisualisationMode = false);
									RenderCommandMesh(const RenderCommandMesh&) = delete;
									RenderCommandMesh(RenderCommandMesh&&) = delete;
									~RenderCommandMesh() override = default;

			void					operator=(const RenderCommandMesh&) = delete;
			void					operator=(RenderCommandMesh&&) = delete;

		public:

			void					Execute(CommandBuffer* commandBuffer) override;

		protected:

			std::vector<uint8_t>	_vertices;
			uint32_t				_vertexCount;

			std::vector<uint16_t>	_indices;
			Matrix4					_modelMatrix;
			const MaterialInstance*	_materialInstance = nullptr;
			bool					_ignoreVisualisationMode;
		};
	}
}
