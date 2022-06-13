
#include "RenderCommandImGui.h"

#include "RHI/Buffer.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <cstring>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandImGui::RenderCommandImGui(const std::vector<DrawList*>& drawLists)
			: RenderCommand()
			, _drawLists(drawLists)
		{
		}

		// TODO delete in destructor

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandImGui::Execute(CommandBuffer* commandBuffer)
		{
			Renderer* renderer = Renderer::GetInstance();

			for (int drawListIndex = 0; drawListIndex < _drawLists.size(); ++drawListIndex)
			{
				DrawList* drawList = _drawLists[drawListIndex];

				Buffer* vertexBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex);

				uint32_t vertexBufferSize = static_cast<uint32_t>(drawList->_vertices.size());
				if (vertexBuffer->Resize(vertexBufferSize) == false)
				{
					delete vertexBuffer;
					return;
				}

				void* vertexBufferData = vertexBuffer->Lock();
				if (vertexBufferData != nullptr)
				{
					memcpy(vertexBufferData, drawList->_vertices.data(), vertexBufferSize);
					vertexBuffer->Unlock();
				}
				commandBuffer->DeleteAfterRender(vertexBuffer);

				Buffer* indexBuffer = renderer->CreateBuffer(Buffer::Usage::Index);

				uint32_t indexBufferSize = static_cast<uint32_t>(drawList->_indices.size() * sizeof(uint16_t));
				if (indexBuffer->Resize(indexBufferSize) == false)
				{
					delete indexBuffer;
					return;
				}

				void* indexBufferData = indexBuffer->Lock();
				if (indexBufferData != nullptr)
				{
					memcpy(indexBufferData, drawList->_indices.data(), indexBufferSize);
					indexBuffer->Unlock();
				}
				commandBuffer->DeleteAfterRender(indexBuffer);

				for (int commandIndex = 0; commandIndex < drawList->_commands.size(); ++commandIndex)
				{
					Command& command = drawList->_commands[commandIndex];

					commandBuffer->SetIndexBuffer(indexBuffer, command._vertexOffset);
					commandBuffer->SetVertexBuffer(vertexBuffer, command._indexOffset);
					commandBuffer->SetScissor(command._clipRect);
					//commandBuffer->SetMaterialInstance(_materialInstance, 0);
					commandBuffer->DrawIndexed(command._elementCount);
				}
			}
		}
	}
}
