#include "HodEngine/ImGui/RenderCommandImGui.h"
#include "HodEngine/ImGui/ImGuiManager.h"

#include "HodEngine/Renderer/RHI/Buffer.h"
#include "HodEngine/Renderer/RHI/CommandBuffer.h"

#include "HodEngine/Renderer/Renderer.h"

#include <cstring>

#include "HodEngine/Renderer/RHI/VertexInput.h"
#include "HodEngine/Renderer/RHI/MaterialInstance.h"

#include "HodEngine/Core/Rect.h"

namespace hod::imgui
{
	/// @brief 
	/// @param drawLists 
	/// @param viewport 
	RenderCommandImGui::RenderCommandImGui(const std::vector<DrawList*>& drawLists, const Rect& viewport)
		: RenderCommand()
		, _drawLists(drawLists)
		, _viewport(viewport)
	{
	}

	/// @brief 
	/// @param commandBuffer 
	void RenderCommandImGui::Execute(renderer::CommandBuffer* commandBuffer)
	{
		renderer::Renderer* renderer = renderer::Renderer::GetInstance();

		commandBuffer->SetViewport(_viewport);

		struct Constant
		{
			Vector2	_scale;
			Vector2	_translate;
		};

		for (int drawListIndex = 0; drawListIndex < _drawLists.size(); ++drawListIndex)
		{
			DrawList* drawList = _drawLists[drawListIndex];
			renderer::Buffer* vertexBuffer = renderer->CreateBuffer(renderer::Buffer::Usage::Vertex);

			uint32_t vertexBufferSize = static_cast<uint32_t>(drawList->_vertices.size() * sizeof(Vertex));
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
			commandBuffer->SetVertexBuffer(vertexBuffer, 0);

			renderer::Buffer* indexBuffer = renderer->CreateBuffer(renderer::Buffer::Usage::Index);

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
			commandBuffer->SetIndexBuffer(indexBuffer, 0);

			Constant constant;
			constant._scale.SetX(2.0f / drawList->_displaySize.GetX());
			constant._scale.SetY(2.0f / drawList->_displaySize.GetY());
			constant._translate.SetX(-1.0f - drawList->_displayPosition.GetX() * constant._scale.GetX());
			constant._translate.SetY(-1.0f - drawList->_displayPosition.GetY() * constant._scale.GetY());

			for (int commandIndex = 0; commandIndex < drawList->_commands.size(); ++commandIndex)
			{
				Command& command = drawList->_commands[commandIndex];

				renderer::MaterialInstance* materialInstance = renderer->CreateMaterialInstance(ImGuiManager::GetInstance()->GetMaterial());

				materialInstance->SetTexture("sTexture", command._texture);
				commandBuffer->SetMaterialInstance(materialInstance, 0);
				commandBuffer->SetConstant(&constant, sizeof(constant), renderer::Shader::ShaderType::Vertex);
				commandBuffer->DeleteAfterRender(materialInstance);

				commandBuffer->SetScissor(command._clipRect);
				commandBuffer->DrawIndexed(command._elementCount, command._indexOffset, command._vertexOffset);
			}
		}
	}
}
