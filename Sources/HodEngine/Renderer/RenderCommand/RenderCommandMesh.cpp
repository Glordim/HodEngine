#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/MaterialInstance.hpp"

#include "HodEngine/Renderer/FrameResources.hpp"
#include "HodEngine/Renderer/PickingManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"

#include <HodEngine/Core/Time/SystemTime.hpp>
#include <HodEngine/Core/StaticArray.hpp>

#include <cassert>
#include <cstring>

namespace hod::inline renderer
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
	RenderCommandMesh::RenderCommandMesh(const Vector2* positions, const Vector2* uvs, const Color* colors, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount,
	                                     const Matrix4& modelMatrix, const MaterialInstance* materialInstance, uint32_t order, uint32_t pickingId, bool ignoreVisualisationMode)
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
		_positions.Resize(vertexCount);
		memcpy(_positions.Data(), positions, sizeof(Vector2) * vertexCount);

		if (uvs != nullptr)
		{
			_uvs.Resize(vertexCount);
			memcpy(_uvs.Data(), uvs, sizeof(Vector2) * vertexCount);
		}

		if (colors != nullptr)
		{
			_colors.Resize(vertexCount);
			memcpy(_colors.Data(), colors, sizeof(Color) * vertexCount);
		}

		if (indices != nullptr)
		{
			memcpy(_indices.Data(), indices, indexCount * sizeof(uint16_t));
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

		MaterialInstance* materialInstance = const_cast<MaterialInstance*>(_materialInstance);
		materialInstance->SetMat4("global.view", commandBuffer->_view);
		materialInstance->SetMat4("global.proj", commandBuffer->_projection);
		materialInstance->SetFloat("global.time", (float)SystemTime::ToSeconds(SystemTime::Now()));
		if (overrideMaterial != nullptr)
		{
			Color color = PickingManager::ConvertIdToColor(_pickingId);

			materialInstance = MaterialInstance::Create(&overrideMaterial->GetMaterial());
			materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));
			Renderer::GetInstance()->GetCurrentFrameResources().DeleteAfter(materialInstance);
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
		materialInstance->Bind(*commandBuffer);

		StaticArray<Buffer*, 3> vertexBuffers = {nullptr, nullptr, nullptr};
		uint32_t               vertexBufferCount = 0;

		Buffer* positionsBuffer = RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_positions.Size() * sizeof(Vector2));
		void*   positionsBufferData = positionsBuffer->Lock();
		if (positionsBufferData != nullptr)
		{
			memcpy(positionsBufferData, _positions.Data(), _positions.Size() * sizeof(Vector2));
			positionsBuffer->Unlock();
		}
		vertexBuffers[vertexBufferCount] = positionsBuffer;
		commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
		++vertexBufferCount;

		if (_uvs.Empty() == false)
		{
			Buffer* uvsBuffer = RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_uvs.Size() * sizeof(Vector2));
			void*   uvsBufferData = uvsBuffer->Lock();
			if (uvsBufferData != nullptr)
			{
				memcpy(uvsBufferData, _uvs.Data(), _uvs.Size() * sizeof(Vector2));
				uvsBuffer->Unlock();
			}
			vertexBuffers[vertexBufferCount] = uvsBuffer;
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
			++vertexBufferCount;
		}

		if (_colors.Empty() == false)
		{
			Buffer* colorsBuffer = RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Vertex, (uint32_t)_colors.Size() * sizeof(Color));
			void*   colorsBufferData = colorsBuffer->Lock();
			if (colorsBufferData != nullptr)
			{
				memcpy(colorsBufferData, _colors.Data(), _colors.Size() * sizeof(Color));
				colorsBuffer->Unlock();
			}
			vertexBuffers[vertexBufferCount] = colorsBuffer;
			commandBuffer->DeleteAfterRender(vertexBuffers[vertexBufferCount]);
			++vertexBufferCount;
		}

		commandBuffer->SetVertexBuffer(vertexBuffers.Data(), vertexBufferCount);

		if (_indices.Empty() == false)
		{
			uint32_t indexBufferSize = static_cast<uint32_t>(_indices.Size() * sizeof(uint16_t));
			Buffer*  indexBuffer = RhiDevice::GetInstance()->CreateBuffer(Buffer::Usage::Index, indexBufferSize);
			void*    indexBufferData = indexBuffer->Lock();
			if (indexBufferData != nullptr)
			{
				memcpy(indexBufferData, _indices.Data(), indexBufferSize);
				indexBuffer->Unlock();
			}
			commandBuffer->SetIndexBuffer(indexBuffer);
			commandBuffer->DeleteAfterRender(indexBuffer);
		}

		commandBuffer->SetModelMatrix(_modelMatrix);

		struct Constant
		{
			Matrix4 _mvp;
			Matrix4 _model;
		};

		Constant constant;
		constant._mvp = (commandBuffer->_projection * commandBuffer->_view * _modelMatrix).Transpose();
		constant._model = _modelMatrix.Transpose();
		commandBuffer->SetConstant(&constant, sizeof(constant), Shader::ShaderType::Vertex);

		if (_indices.Empty() == false)
		{
			commandBuffer->DrawIndexed((uint32_t)_indices.Size(), 0, 0);
		}
		else
		{
			commandBuffer->Draw(_vertexCount);
		}

		if (_ignoreVisualisationMode == false)
		{
			if (renderer->GetVisualizationMode() == Renderer::VisualizationMode::NormalWithWireframe)
			{
				Renderer::GetInstance()->GetWireframeMaterialInstance()->Bind(*commandBuffer);
				commandBuffer->DrawIndexed((uint32_t)_indices.Size(), 0, 0);
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
