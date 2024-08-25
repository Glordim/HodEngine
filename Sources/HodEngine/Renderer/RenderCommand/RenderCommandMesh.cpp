#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

#include <cstring>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandMesh::RenderCommandMesh(const void* vertices, uint32_t vertexCount, uint32_t vertexSize, const uint16_t* indices, uint32_t indexCount, const Matrix4& modelMatrix, const MaterialInstance* materialInstance, bool ignoreVisualisationMode)
			: RenderCommand()
			, _vertices(vertexCount * vertexSize)
			, _vertexCount(vertexCount)
			, _indices(indexCount)
			, _modelMatrix(modelMatrix)
			, _materialInstance(materialInstance)
			, _ignoreVisualisationMode(ignoreVisualisationMode)
		{
			memcpy(_vertices.data(), vertices, vertexCount * vertexSize);
			memcpy(_indices.data(), indices, indexCount * sizeof(uint16_t));

			if (materialInstance == nullptr)
			{
				_materialInstance = Renderer::GetInstance()->GetDefaultMaterialInstance();
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandMesh::Execute(CommandBuffer* commandBuffer)
		{
			Renderer* renderer = Renderer::GetInstance();

            uint32_t vertexBufferSize = static_cast<uint32_t>(_vertices.size());
			Buffer* vertexBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, vertexBufferSize);
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
                uint32_t indexBufferSize = static_cast<uint32_t>(_indices.size() * sizeof(uint16_t));
				Buffer* indexBuffer = renderer->CreateBuffer(Buffer::Usage::Index, indexBufferSize);
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
					commandBuffer->SetMaterialInstance(_materialInstance, 0);
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
				commandBuffer->SetMaterialInstance(_materialInstance, 0);
			}

			struct Constant
			{
				Matrix4	_mvp;
			};

			Constant constant;
			constant._mvp = commandBuffer->_projection * commandBuffer->_view * _modelMatrix;
			commandBuffer->SetConstant(&constant, sizeof(constant), renderer::Shader::ShaderType::Vertex);

			if (_indices.empty() == false)
			{
				commandBuffer->DrawIndexed((uint32_t)_indices.size(), 0, 0);
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
					commandBuffer->DrawIndexed((uint32_t)_indices.size(), 0, 0);
				}
			}
		}
	}
}
