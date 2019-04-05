#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>

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

    stbi_set_flip_vertically_on_load(true);

    unsigned char* textureBuffer = stbi_load(path, &width, &height, &channel, 3);
    if (textureBuffer == nullptr)
    {
        std::cerr << std::string("Texture : Failed to load Texture \"") + path + "\"" << std::endl;
        return false;
    }

    glGenTextures(1, &this->texureId);
    glBindTexture(GL_TEXTURE_2D, this->texureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(textureBuffer);

    return true;
}

GLuint Texture::getTextureId() const
{
    return this->texureId;
}
