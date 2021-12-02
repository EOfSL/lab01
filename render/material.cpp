#include "material.h"

material::material(const texture& a_texture, const shader& a_shader) noexcept
    : m_texture(a_texture), m_shader(a_shader)
{}

material::material(const std::string& a_texture, const std::string& a_shader) noexcept
    : m_texture(a_texture), m_shader(a_shader)
{}

void material::update(const camera& a_camera) const noexcept
{
    m_shader.update(a_camera);
}

void material::bind() const noexcept
{
    m_texture.bind();
    m_shader.bind();
}
