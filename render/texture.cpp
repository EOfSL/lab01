#include "texture.h"

#include "stb_image.h"
#include <iostream>


texture::texture(const std::string& a_file) noexcept
{
    int width, heigth, numComponents;
    unsigned char* image = stbi_load((a_file).c_str(), &width, &heigth, &numComponents, 4);

    if (image == nullptr)
        std::cerr << "Texture loading failed at file " << a_file << std::endl;

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Повторение текстур дальше uv-квадрата (0,0)-(1,1)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Линейная фильтрация GL_LINEAR
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Сгенерировать мипмапы
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
}

texture::~texture() noexcept
{
    glDeleteTextures(1, &m_texture);
}

void texture::bind() const noexcept
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
