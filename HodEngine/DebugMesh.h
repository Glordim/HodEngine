#ifndef __DEBUG_MESH_HPP__
#define __DEBUG_MESH_HPP__

#include <vector>

#include "Tri_3P_3C.h"

class CameraComponent;

class DebugMesh
{
public:
    DebugMesh();
    virtual ~DebugMesh();

    void buildVao(const std::vector<Tri_3P_3C>& tris);
    void draw(CameraComponent* cameraComponent);

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int vertexCount;
};

#endif // __DEBUG_LINE_HPP__
