#pragma once

#include "../Mesh.h"

#include <vulkan/vulkan.h>

namespace HOD
{
    class VkMesh : public Mesh
    {
    public:
        VkMesh();
        virtual ~VkMesh();

        VkBuffer GetVertexBuffer() const;
        VkBuffer GetIndiceBuffer() const;

    protected:
        virtual bool BuildBuffers() override;

    private:

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer indiceBuffer;
        VkDeviceMemory indiceBufferMemory;
    };
}
