#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

#include "transform.h"
#include "camera.h"


class shader
{
public:
    explicit shader(const std::string&);

    void bind() const noexcept;
    void update(const camera&) const noexcept;

    virtual ~shader();

private:

    enum {
        TRANSFORM_U,
        DIFFUSE_U,
        NUM_UNIFORMS
    };

    // Шейдерная программа
    static const size_t NUM_SHADERS = 2; // Вертексный и фрагментный
    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
    GLuint m_uniforms[NUM_UNIFORMS];
};

#endif // SHADER_H
