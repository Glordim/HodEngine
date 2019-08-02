#ifndef __DEBUG_LINE_HPP__
#define __DEBUG_LINE_HPP__

#include <vector>

#include "Line_3P_3C.h"

class CameraComponent;

class DebugLine
{
public:
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
