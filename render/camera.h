#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtx/transform.hpp>
#include "geometry/vec3d.h"


class camera
{
public:

    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    glm::mat4 m_projection_matrix;

    camera() noexcept;
    camera(glm::vec3, float, float, float, float) noexcept;

    glm::vec3 right() const noexcept;
    glm::mat4 view_matrix() const noexcept;
    glm::mat4 view_projection_matrix() const noexcept;

    const float g_modifer = 0.1f;

    void move_forward() noexcept;
    void move_backward() noexcept;
    void move_left() noexcept;
    void move_right() noexcept;
    void move_up() noexcept;
    void move_down() noexcept;

    vec3d pos() const noexcept;
};

#endif // CAMERA_H
