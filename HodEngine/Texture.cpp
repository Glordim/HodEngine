#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{
}

Texture::~Texture()
{
    if (this->texureId != 0)
        glDeleteTextures(1, &this->texureId);
}

bool Texture::load(const char* path)
{
    int width;
    int height;
    int channel;
    unsigned char* textureBuffer = stbi_load(path, &width, &height, &channel, 3);
    if (textureBuffer == nullptr)
        return false;

    glGenTextures(1, &this->texureId);
    glBindTexture(GL_TEXTURE_2D, this->texureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return true;
}

GLuint Texture::getTextureId() const
{
    return this->texureId;
}
