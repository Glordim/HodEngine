#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "glad/glad.h"

Mesh::Mesh()
{
    for (int i = 0; i < 4; ++i)
    {
        this->vbo[i] = 0;
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(4, &this->vbo[0]);
}

bool Mesh::loadObj(const char* path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path) == false)
        return false;

    unsigned int index = 0;

    for (int i = 0; i < shapes.size(); ++i)
    {
        tinyobj::shape_t& shape = shapes[i];

        for (int j = 0; j < shape.mesh.indices.size(); ++j)
        {
            int verticesIndex = shape.mesh.indices[j].vertex_index;

            this->vertices.push_back(attributes.vertices[(verticesIndex * 3)]);
            this->vertices.push_back(attributes.vertices[(verticesIndex * 3) + 1]);
            this->vertices.push_back(attributes.vertices[(verticesIndex * 3) + 2]);

            int uvIndex = shape.mesh.indices[j].texcoord_index;

            if (uvIndex != -1)
            {
                this->uv.push_back(attributes.texcoords[(uvIndex * 2)]);
                this->uv.push_back(1.0f - attributes.texcoords[(uvIndex * 2) + 1]);
            }
            else
            {
                this->uv.push_back(0.0f);
                this->uv.push_back(0.0f);
            }

            int normalIndex = shape.mesh.indices[j].normal_index;

            if (normalIndex != -1)
            {
                this->normal.push_back(attributes.normals[(normalIndex * 3)]);
                this->normal.push_back(attributes.normals[(normalIndex * 3) + 1]);
                this->normal.push_back(attributes.normals[(normalIndex * 3) + 2]);
            }
            else
            {
                this->normal.push_back(0.0f);
                this->normal.push_back(0.0f);
                this->normal.push_back(1.0f);
            }

            this->indices.push_back(index);
            ++index;
        }
    }

    this->buildVao();

    return true;
}

void Mesh::buildVao()
{
    glGenBuffers(4, &this->vbo[0]);
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), &this->vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Uv
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, this->uv.size() * sizeof(float), &this->uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Normal
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, this->normal.size() * sizeof(float), &this->normal[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Indice
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);
}

unsigned int Mesh::getVao() const
{
    return this->vao;
}

unsigned int Mesh::getIndicesCount() const
{
    return this->indices.size();
}
