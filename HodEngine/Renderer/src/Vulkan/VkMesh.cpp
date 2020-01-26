#include "VkMesh.h"

#include "RendererVulkan.h"

namespace HOD
{
    VkMesh::VkMesh()
    {
        this->vertexBuffer = VK_NULL_HANDLE;
        this->vertexBufferMemory = VK_NULL_HANDLE;

        this->indiceBuffer = VK_NULL_HANDLE;
        this->indiceBufferMemory = VK_NULL_HANDLE;
    }

    VkMesh::~VkMesh()
    {
        RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

        if (this->vertexBuffer != VK_NULL_HANDLE)
            vkDestroyBuffer(renderer->GetVkDevice(), this->vertexBuffer, nullptr);
        if (this->vertexBufferMemory != VK_NULL_HANDLE)
            vkFreeMemory(renderer->GetVkDevice(), this->vertexBufferMemory, nullptr);

        if (this->indiceBuffer != VK_NULL_HANDLE)
            vkDestroyBuffer(renderer->GetVkDevice(), this->indiceBuffer, nullptr);
        if (this->indiceBufferMemory != VK_NULL_HANDLE)
            vkFreeMemory(renderer->GetVkDevice(), this->indiceBufferMemory, nullptr);
    }

    VkBuffer VkMesh::GetVertexBuffer() const
    {
        return this->vertexBuffer;
    }

    VkBuffer VkMesh::GetIndiceBuffer() const
    {
        return this->indiceBuffer;
    }

    bool VkMesh::BuildBuffers()
    {
        RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

        VkDeviceSize bufferSize = sizeof(this->vertices[0]) * this->vertices.size();

        if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->vertexBuffer, &this->vertexBufferMemory) == false)
            return false;

        void* data = nullptr;

        if (vkMapMemory(renderer->GetVkDevice(), this->vertexBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
        {
            fprintf(stderr, "Vulkan: Unable to map vertex buffer memory!\n");
            return false;
        }
        memcpy(data, this->vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(renderer->GetVkDevice(), this->vertexBufferMemory);

        // ---------------

        if (this->indices.empty() == false)
        {
            bufferSize = sizeof(this->indices[0]) * this->indices.size();

            if (renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->indiceBuffer, &this->indiceBufferMemory) == false)
                return false;

            data = nullptr;

            if (vkMapMemory(renderer->GetVkDevice(), this->indiceBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
            {
                fprintf(stderr, "Vulkan: Unable to map indice buffer memory!\n");
                return false;
            }
            memcpy(data, this->indices.data(), (size_t)bufferSize);
            vkUnmapMemory(renderer->GetVkDevice(), this->indiceBufferMemory);
        }

        return true;
    }
}
