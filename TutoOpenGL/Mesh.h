#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    bool loadObj(const char* path);

    unsigned int getVbo(int index) const;
    unsigned int getIndicesCount() const;

private:
    void buildVBO();

private:
    std::vector<unsigned int> indices;
    std::vector<float> vertices;
    std::vector<float> uv;
    std::vector<float> normal;

    unsigned int vbo[4];
};

#endif
