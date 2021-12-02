#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include "texture.h"


class material
{
    texture m_texture;
    shader m_shader;

public:
    material(const texture&, const shader&) noexcept;
    explicit material(const std::string&, const std::string& = "./shaders/texturedShader") noexcept;

    void update(const camera&) const noexcept;
    void bind() const noexcept;
};

#endif // MATERIAL_H
