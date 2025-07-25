#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/RenderCommandImGui.hpp"
#include "HodEngine/ImGui/ImGuiManager.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

#include <cstring>

#include "HodEngine/Renderer/RHI/VertexInput.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Core/Rect.hpp"

namespace hod::imgui
{
	/// @brief 
	/// @param drawLists 
	/// @param viewport 
	RenderCommandImGui::RenderCommandImGui(const Vector<DrawList*>& drawLists, const Rect& viewport)
		: RenderCommand()
		, _viewport(viewport)
		, _drawLists(drawLists)
	{
	}

	/// @brief 
	RenderCommandImGui::~RenderCommandImGui()
	{
		for (DrawList* drawList : _drawLists)
		{
			DefaultAllocator::GetInstance().Delete(drawList);
		}
	}

	/// @brief 
	/// @param commandBuffer 
	void RenderCommandImGui::Execute(renderer::CommandBuffer* commandBuffer, renderer::MaterialInstance* overrideMaterial)
	{
		if (overrideMaterial != nullptr)
		{
			return;
		}

		renderer::Renderer* renderer = renderer::Renderer::GetInstance();

		commandBuffer->SetViewport(_viewport);

		struct Constant
		{
			Vector2	_scale;
			Vector2	_translate;
		};

		for (size_t drawListIndex = 0; drawListIndex < _drawLists.size(); ++drawListIndex)
		{
			DrawList* drawList = _drawLists[drawListIndex];
			
            uint32_t vertexBufferSize = static_cast<uint32_t>(drawList->_vertices.size() * sizeof(Vertex));
			renderer::Buffer* vertexBuffer = renderer->CreateBuffer(renderer::Buffer::Usage::Vertex, vertexBufferSize);
			void* vertexBufferData = vertexBuffer->Lock();
			if (vertexBufferData != nullptr)
			{
				memcpy(vertexBufferData, drawList->_vertices.data(), vertexBufferSize);
				vertexBuffer->Unlock();
			}
			commandBuffer->DeleteAfterRender(vertexBuffer);
			commandBuffer->SetVertexBuffer(&vertexBuffer, 1);

            uint32_t indexBufferSize = static_cast<uint32_t>(drawList->_indices.size() * sizeof(uint16_t));
			renderer::Buffer* indexBuffer = renderer->CreateBuffer(renderer::Buffer::Usage::Index, indexBufferSize);
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

			for (size_t commandIndex = 0; commandIndex < drawList->_commands.size(); ++commandIndex)
			{
				Command& command = drawList->_commands[commandIndex];

				renderer::MaterialInstance* materialInstance = renderer->CreateMaterialInstance(ImGuiManager::GetInstance()->GetMaterial());

				materialInstance->SetTexture("image", command._texture);
				commandBuffer->SetMaterialInstance(materialInstance, 0);
				commandBuffer->SetConstant(&constant, sizeof(constant), renderer::Shader::ShaderType::Vertex);
				commandBuffer->DeleteAfterRender(materialInstance);

				commandBuffer->SetScissor(command._clipRect);
				commandBuffer->DrawIndexed(command._elementCount, command._indexOffset, command._vertexOffset);
			}
		}
	}

	uint32_t RenderCommandImGui::GetRenderingOrder() const
	{
		return 0;
	}
}
