#include "DebugLine.h"

#include "MaterialManager.h"
#include "Material.h"

DebugLine::DebugLine()
    : vao(0)
    , vbo(0)
{

}

DebugLine::~DebugLine()
{
    /*

    if (this->vao != 0)
        glDeleteVertexArrays(1, &this->vao);
    if (this->vbo != 0)
        glDeleteBuffers(1, &this->vbo);

    */
}

void DebugLine::buildVao(const std::vector<Line_3P_3C>& lines)
{
    /*

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Line_3P_3C) * lines.size(), &lines[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    this->vertexCount = lines.size() * 2;

    */
}

void DebugLine::draw(CameraComponent* cameraComponent)
{
    /*

    Material* material = MaterialManager::getInstance()->getMaterial("UnlitVertexColor");
    material->use();

    glBindVertexArray(this->vao);
    glDrawArrays(GL_LINES, 0, this->vertexCount);

    */
}
