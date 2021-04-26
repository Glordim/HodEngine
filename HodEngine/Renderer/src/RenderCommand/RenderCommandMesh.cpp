
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
		RenderCommandMesh::RenderCommandMesh(const void* vertices, uint32_t vertexCount, uint32_t vertexSize, const uint16_t* indices, uint32_t indexCount, const glm::mat4x4& modelMatrix, MaterialInstance* materialInstance, bool ignoreVisualisationMode)
			: RenderCommand()
			, _vertices(vertexCount * vertexSize)
			, _vertexCount(vertexCount)
			, _vertexSize(vertexSize)
			, _indices(indexCount)
			, _materialInstance(materialInstance)
			, _modelMatrix(modelMatrix)
			, _ignoreVisualisationMode(ignoreVisualisationMode)
		{
			memcpy(_vertices.data(), vertices, vertexCount * vertexSize);
			memcpy(_indices.data(), indices, sizeof(uint16_t) * indexCount);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandMesh::Execute(CommandBuffer* commandBuffer)
		{
			Renderer* renderer = Renderer::GetInstance();

			Buffer* vertexBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex);

			uint32_t vertexBufferSize = static_cast<uint32_t>(_vertices.size());
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
			commandBuffer->SetVertexBuffer(vertexBuffer);
			commandBuffer->DeleteAfterRender(vertexBuffer);

			if (_indices.empty() == false)
			{
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
				commandBuffer->SetIndexBuffer(indexBuffer);
				commandBuffer->DeleteAfterRender(indexBuffer);
			}

			commandBuffer->SetModelMatrix(_modelMatrix);

			if (_ignoreVisualisationMode == false)
			{
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
			}
			else
			{
				commandBuffer->SetMaterialInstance(_materialInstance);
			}

			if (_indices.empty() == false)
			{
				commandBuffer->DrawIndexed(_indices.size());
			}
			else
			{
				commandBuffer->Draw(_vertexCount);
			}

			if (_ignoreVisualisationMode == false)
			{
				if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::NormalWithWireframe)
				{
					commandBuffer->SetMaterialInstance(Renderer::GetInstance()->GetWireframeMaterialInstance());
					commandBuffer->DrawIndexed(_indices.size());
				}
			}
		}
	}
}
