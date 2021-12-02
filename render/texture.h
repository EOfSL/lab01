#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>


class texture
{
public:
    GLuint m_texture;

    explicit texture(const std::string&) noexcept;
    ~texture() noexcept;

    void bind() const noexcept;
};

#endif // TEXTURE_H
