
#include "RenderCommandMesh.h"

#include "RHI/Buffer.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandMesh::RenderCommandMesh(const P2fT2f* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const glm::mat4x4& modelMatrix, MaterialInstance* materialInstance)
			: RenderCommand()
			, _vertices(vertexCount)
			, _indices(indexCount)
			, _materialInstance(materialInstance)
			, _modelMatrix(modelMatrix)
		{
			memcpy(_vertices.data(), vertices, sizeof(P2fT2f) * vertexCount);
			memcpy(_indices.data(), indices, sizeof(uint16_t) * indexCount);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandMesh::Execute(CommandBuffer* commandBuffer)
		{
			Renderer* renderer = Renderer::GetInstance();

			Buffer* vertexBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex);

			uint32_t vertexBufferSize = static_cast<uint32_t>(_vertices.size() * sizeof(P2fT2f));
			if (vertexBuffer->Resize(vertexBufferSize) == false)
			{
				delete vertexBuffer;
				return;
			}

			void* vertexBufferData = vertexBuffer->Lock();
			if (vertexBufferData != nullptr)
			{
				memcpy(vertexBufferData, _vertices.data(), vertexBufferSize);
				vertexBuffer->Unlock();
			}

			Buffer* indexBuffer = renderer->CreateBuffer(Buffer::Usage::Index);

			uint32_t indexBufferSize = static_cast<uint32_t>(_indices.size() * sizeof(uint16_t));
			if (indexBuffer->Resize(indexBufferSize) == false)
			{
				delete indexBuffer;
				return;
			}

			void* indexBufferData = indexBuffer->Lock();
			if (indexBufferData != nullptr)
			{
				memcpy(indexBufferData, _indices.data(), indexBufferSize);
				indexBuffer->Unlock();
			}

			commandBuffer->SetVertexBuffer(vertexBuffer);
			commandBuffer->SetIndexBuffer(indexBuffer);

			commandBuffer->SetModelMatrix(_modelMatrix);

			if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::Normal || renderer->GetVisualizationMode() == Renderer::VisualizationMode::NormalWithWireframe)
			{
				commandBuffer->SetMaterialInstance(_materialInstance);
			}
			else if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::Wireframe)
			{
				commandBuffer->SetMaterialInstance(Renderer::GetInstance()->GetWireframeMaterialInstance());
			}
			else
			{
				commandBuffer->SetMaterialInstance(Renderer::GetInstance()->GetOverdrawMaterialInstance());
			}
			commandBuffer->DrawIndexed(_indices.size());

			if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::NormalWithWireframe)
			{
				commandBuffer->SetMaterialInstance(Renderer::GetInstance()->GetWireframeMaterialInstance());
				commandBuffer->DrawIndexed(_indices.size());
			}

			//delete vertexBuffer;
		}
	}
}
