#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/PickingManager.hpp"

#include <HodEngine/Core/Time/SystemTime.hpp>

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
	RenderCommandMesh::RenderCommandMesh(const Vector2* positions, const Vector2* uvs, const Color* colors, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Matrix4& modelMatrix, const MaterialInstance* materialInstance, uint32_t order, uint32_t pickingId, bool ignoreVisualisationMode)
		: RenderCommand()
		, _vertexCount(vertexCount)
		, _order(order)
		, _pickingId(pickingId)
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
			memcpy(_colors.data(), colors, sizeof(Color) * vertexCount);
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
	void RenderCommandMesh::Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial)
	{
		if (overrideMaterial != nullptr && _pickingId == PickingManager::InvalidId)
		{
			return;
		}
		
		Renderer* renderer = Renderer::GetInstance();

		std::array<Buffer*, 3> vertexBuffers = { nullptr, nullptr, nullptr };
		uint32_t vertexBufferCount = 0;

		Buffer* positionsBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_positions.size() * sizeof(Vector2));
		void* positionsBufferData = positionsBuffer->Lock();
		if (positionsBufferData != nullptr)
		{
			memcpy(positionsBufferData, _positions.data(), _positions.size() * sizeof(Vector2));
			positionsBuffer->Unlock();
		}
		vertexBuffers[vertexBufferCount] = positionsBuffer;
		commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
		++vertexBufferCount;

		if (_uvs.empty() == false)
		{
			Buffer* uvsBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_uvs.size() * sizeof(Vector2));
			void* uvsBufferData = uvsBuffer->Lock();
			if (uvsBufferData != nullptr)
			{
				memcpy(uvsBufferData, _uvs.data(), _uvs.size() * sizeof(Vector2));
				uvsBuffer->Unlock();
			}
			vertexBuffers[vertexBufferCount] = uvsBuffer;
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
			++vertexBufferCount;
		}

		if (_colors.empty() == false)
		{
			Buffer* colorsBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_colors.size() * sizeof(Color));
			void* colorsBufferData = colorsBuffer->Lock();
			if (colorsBufferData != nullptr)
			{
				memcpy(colorsBufferData, _colors.data(), _colors.size() * sizeof(Color));
				colorsBuffer->Unlock();
			}
			vertexBuffers[vertexBufferCount] = colorsBuffer;
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
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

		MaterialInstance* materialInstance = const_cast<MaterialInstance*>(_materialInstance);
		materialInstance->SetMat4("global.view", commandBuffer->_view);
		materialInstance->SetMat4("global.proj", commandBuffer->_projection);
		materialInstance->SetFloat("global.time", (float)SystemTime::ToSeconds(SystemTime::Now()));
		if (overrideMaterial != nullptr)
		{
			Color color = renderer::PickingManager::ConvertIdToColor(_pickingId);

			materialInstance = Renderer::GetInstance()->CreateMaterialInstance(&overrideMaterial->GetMaterial());
			materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));
			commandBuffer->DeleteAfterRender(materialInstance);
		}
		else if (_ignoreVisualisationMode == false)
		{
			if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::Wireframe)
			{
				materialInstance = Renderer::GetInstance()->GetWireframeMaterialInstance();
			}
			else if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::NormalWithWireframe)
			{
				materialInstance = (Renderer::GetInstance()->GetOverdrawMaterialInstance());
			}
		}
		commandBuffer->SetMaterialInstance(materialInstance, 0);

		struct Constant
		{
			Matrix4	_mvp;
			Matrix4 _model;
		};

		Constant constant;
		constant._mvp = (commandBuffer->_projection * commandBuffer->_view * _modelMatrix).Transpose();
		constant._model = _modelMatrix.Transpose();
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

	/// @brief 
	/// @return 
	uint32_t RenderCommandMesh::GetRenderingOrder() const
	{
		return _order;
	}
}
