#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    bool loadObj(const char* path);

    unsigned int getVao() const;
    unsigned int getIndicesCount() const;

private:
    void buildVao();

private:
    std::vector<unsigned int> indices;
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> biTangents;

    unsigned int vbo[4];
    unsigned int vao;
};

#endif
