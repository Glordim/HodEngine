#include "VkMesh.h"

#include "RendererVulkan.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMesh::VkMesh()
		{
			_vertexBuffer = VK_NULL_HANDLE;
			_vertexBufferMemory = VK_NULL_HANDLE;

			_indiceBuffer = VK_NULL_HANDLE;
			_indiceBufferMemory = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkMesh::~VkMesh()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			if (_vertexBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(renderer->GetVkDevice(), _vertexBuffer, nullptr);
			}
			if (_vertexBufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), _vertexBufferMemory, nullptr);
			}

			if (_indiceBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(renderer->GetVkDevice(), _indiceBuffer, nullptr);
			}
			if (_indiceBufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(renderer->GetVkDevice(), _indiceBufferMemory, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkBuffer VkMesh::GetVertexBuffer() const
		{
			return _vertexBuffer;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkBuffer VkMesh::GetIndiceBuffer() const
		{
			return _indiceBuffer;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool VkMesh::BuildBuffers()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

			if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &_vertexBuffer, &_vertexBufferMemory) == false)
				return false;

			void* data = nullptr;

			if (vkMapMemory(renderer->GetVkDevice(), _vertexBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
			{
				fprintf(stderr, "Vulkan: Unable to map vertex buffer memory!\n");
				return false;
			}
			memcpy(data, _vertices.data(), (size_t)bufferSize);
			vkUnmapMemory(renderer->GetVkDevice(), _vertexBufferMemory);

			// ---------------

			if (_indices.empty() == false)
			{
				bufferSize = sizeof(_indices[0]) * _indices.size();

				if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &_indiceBuffer, &_indiceBufferMemory) == false)
				{
					return false;
				}

				data = nullptr;

				if (vkMapMemory(renderer->GetVkDevice(), _indiceBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
				{
					fprintf(stderr, "Vulkan: Unable to map indice buffer memory!\n");
					return false;
				}
				memcpy(data, _indices.data(), (size_t)bufferSize);
				vkUnmapMemory(renderer->GetVkDevice(), _indiceBufferMemory);
			}

			return true;
		}
	}
}
