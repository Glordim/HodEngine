#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

#include <cstring>
#include <cassert>

namespace hod::renderer
{
	/// @brief 
	/// @param positions 
	/// @param uvs 
	/// @param colors 
	/// @param vertexCount 
	/// @param indices 
	/// @param indexCount 
	/// @param modelMatrix 
	/// @param materialInstance 
	/// @param ignoreVisualisationMode 
	RenderCommandMesh::RenderCommandMesh(const Vector2* positions, const Vector2* uvs, const Color* colors, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Matrix4& modelMatrix, const MaterialInstance* materialInstance, bool ignoreVisualisationMode)
		: RenderCommand()
		, _vertexCount(vertexCount)
		, _indices(indexCount)
		, _modelMatrix(modelMatrix)
		, _materialInstance(materialInstance)
		, _ignoreVisualisationMode(ignoreVisualisationMode)
	{
		assert(positions != nullptr);
		_positions.resize(vertexCount);
		memcpy(_positions.data(), positions, sizeof(Vector2) * vertexCount);

		if (uvs != nullptr)
		{
			_uvs.resize(vertexCount);
			memcpy(_uvs.data(), uvs, sizeof(Vector2) * vertexCount);
		}

		if (colors != nullptr)
		{
			_colors.resize(vertexCount);
			memcpy(_colors.data(), uvs, sizeof(Color) * vertexCount);
		}

		if (indices != nullptr)
		{
			memcpy(_indices.data(), indices, indexCount * sizeof(uint16_t));
		}

		if (materialInstance == nullptr)
		{
			_materialInstance = Renderer::GetInstance()->GetDefaultMaterialInstance();
		}
	}

	/// @brief 
	/// @param commandBuffer 
	void RenderCommandMesh::Execute(CommandBuffer* commandBuffer)
	{
		Renderer* renderer = Renderer::GetInstance();

		std::array<Buffer*, 3> vertexBuffers = { nullptr, nullptr, nullptr };

		vertexBuffers[0] = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_positions.size() * sizeof(Vector2));
		void* positionsBufferData = vertexBuffers[0]->Lock();
		if (positionsBufferData != nullptr)
		{
			memcpy(positionsBufferData, _positions.data(), _positions.size() * sizeof(Vector2));
			vertexBuffers[0]->Unlock();
		}
		commandBuffer->DeleteAfterRender(vertexBuffers[0]);
		uint32_t vertexBufferCount = 1;

		if (_uvs.empty() == false)
		{
			Buffer* uvsBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_uvs.size() * sizeof(Vector2));
			void* uvsBufferData = uvsBuffer->Lock();
			if (uvsBufferData != nullptr)
			{
				memcpy(uvsBufferData, _uvs.data(), _uvs.size() * sizeof(Vector2));
				uvsBuffer->Unlock();
			}
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
			vertexBuffers[vertexBufferCount] = uvsBuffer;
			++vertexBufferCount;
		}

		if (_colors.empty() == false)
		{
			Buffer* colorsBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_colors.size() * sizeof(Color));
			void* colorsBufferData = colorsBuffer->Lock();
			if (colorsBufferData != nullptr)
			{
				memcpy(colorsBufferData, _uvs.data(), _uvs.size() * sizeof(Color));
				colorsBuffer->Unlock();
			}
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
			vertexBuffers[vertexBufferCount] = colorsBuffer;
			++vertexBufferCount;
		}

		commandBuffer->SetVertexBuffer(vertexBuffers.data(), vertexBufferCount);

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
