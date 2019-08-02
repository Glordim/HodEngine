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
}

bool Texture::LoadFromPath(const char* path)
{
    int width;
    int height;
    int channel;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* buffer = stbi_load(path, &width, &height, &channel, 3);
    if (buffer == nullptr)
    {
        fprintf(stderr, "Texture : Failed to load Texture \"%s\"\n", path);
        return false; // Todo Memleak
    }

    if (this->BuildBuffer(width, height, buffer) == false)
        return false; // Todo Memleak

    stbi_image_free(buffer);

    return true;
}
