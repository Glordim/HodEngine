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
    float bitangent[3];
};

struct Line_3P_3C;
struct Tri_3P_3C;

namespace fbxsdk
{
    class FbxNode;
}

class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    bool loadObj(const char* path);
    bool loadFbx(const char* path);
    bool LoadFromLines(std::vector<Line_3P_3C>& lines);
    bool LoadFromTriangles(std::vector<Tri_3P_3C>& triangles);
    bool LoadSkybox();

    size_t GetIndiceCount() const;
    size_t GetVertexCount() const;

    const std::vector<Vertex_3P_3C_3N_2UV_3TA>& GetVertices() const;

protected:
    virtual bool BuildBuffers() = 0;

protected:
    std::vector<uint16_t> indices;
    std::vector<Vertex_3P_3C_3N_2UV_3TA> vertices;

private:
    fbxsdk::FbxNode* FindMeshInFbxNode(fbxsdk::FbxNode* node);
};

#endif
