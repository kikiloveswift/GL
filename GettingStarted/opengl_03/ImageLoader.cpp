//
// Created by liqiang on 2025/9/2.
//

#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <third_party/stb_image.h>
#include "Utils/Logger.h"

GLuint ImageLoader::loadTexture(const char* path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        LOG_ERROR("Error to load data path = %s", path);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = GL_RGB;
    if (channels == 4) format = GL_RGBA;
    else if (channels == 1) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // data has copy to GPU AND generate Mipmap
    stbi_image_free(data);
    return texture;
}