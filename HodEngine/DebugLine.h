#ifndef __DEBUG_LINE_HPP__
#define __DEBUG_LINE_HPP__

#include <vector>

struct Vertex_3P_3C
{
    float pos[3];
    float color[3];
};

struct Line_3P_3C
{
    Vertex_3P_3C vertices[2];
};

struct Tri_3P_3C
{
    Vertex_3P_3C vertices[3];
};

class CameraComponent;

class DebugLine
{
public:
    DebugLine(const DebugLine& copy) = delete;

    DebugLine();
    virtual ~DebugLine();

    void buildVao(const std::vector<Line_3P_3C>& lines);
    void draw(CameraComponent* cameraComponent);

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int vertexCount;
};

#endif // __DEBUG_LINE_HPP__
