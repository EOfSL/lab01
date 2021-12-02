#include "camera.h"

camera::camera() noexcept
    : camera(glm::vec3(0,0,0), 1.33f, 70.0f, 0.1f, 1000.0f)
{}

camera::camera(glm::vec3 a_pos, float a_aspect, float a_fov, float a_zNear, float a_zFar) noexcept
    : m_position(a_pos),
      m_forward(glm::vec3(0, 0, 1)),
      m_up(glm::vec3(0, 1, 0)),
      m_projection_matrix(glm::perspective(a_fov, a_aspect, a_zNear, a_zFar))
{}

glm::vec3 camera::right() const noexcept
{
    return glm::cross(m_up, m_forward);
}

glm::mat4 camera::view_matrix() const noexcept
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 camera::view_projection_matrix() const noexcept
{
    return m_projection_matrix * view_matrix();
}

void camera::move_forward() noexcept
{
    m_position += m_forward * g_modifer;
}

void camera::move_backward() noexcept
{
    m_position -= m_forward * g_modifer;
}

void camera::move_left() noexcept
{
    m_position -= right() * g_modifer;
}

void camera::move_right() noexcept
{
    m_position += right() * g_modifer;
}

void camera::move_up() noexcept
{
    m_position += m_up * g_modifer;
}

void camera::move_down() noexcept
{
    m_position -= m_up * g_modifer;
}

vec3d camera::pos() const noexcept
{
    return vec3d(m_position.x, m_position.y, m_position.z);
}
