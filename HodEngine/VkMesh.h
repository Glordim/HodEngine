#ifndef __VK_MESH_HPP__
#define __VK_MESH_HPP__

#include  "Mesh.h"

#include <vulkan.h>

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

#endif
