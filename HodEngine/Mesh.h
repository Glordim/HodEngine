#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

struct Vertex_3P_3C_3N_2UV_3TA
{
    float pos[3];
    float color[3];
    float normal[3];
    float uv[2];
    float tangent[3];
};

class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    bool loadObj(const char* path);

    unsigned int GetIndiceCount() const;

protected:
    virtual bool BuildBuffers() = 0;

protected:
    std::vector<uint16_t> indices;
    std::vector<Vertex_3P_3C_3N_2UV_3TA> vertices;
};

#endif
