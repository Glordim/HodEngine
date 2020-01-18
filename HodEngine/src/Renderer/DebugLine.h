#pragma once

#include <vector>

#include "Line_3P_3C.h"

namespace HOD
{
    namespace GAME
    {
        class CameraComponent;
    }

    class DebugLine
    {
    public:
        DebugLine();
        virtual ~DebugLine();

        void buildVao(const std::vector<Line_3P_3C>& lines);
        void draw(GAME::CameraComponent* cameraComponent);

    private:
        unsigned int vao;
        unsigned int vbo;
        unsigned int vertexCount;
    };
}
