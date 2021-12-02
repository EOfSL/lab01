#include "transform.h"

transform::transform(const glm::vec3& a_pos, const glm::quat a_rot, const glm::vec3& a_scale) noexcept
    : m_position(a_pos), m_rotation(a_rot), m_scale(a_scale)
{}

void transform::translate(glm::vec3 a_translation) noexcept
{
    m_position += a_translation;
}

void transform::rotate(float a_angle, glm::vec3 a_axis) noexcept
{
    m_rotation *= glm::angleAxis(a_angle, a_axis);
}

void transform::scaleBy(glm::vec3 a_scale) noexcept
{
    m_scale *= a_scale;
}

glm::mat4 transform::calculate_transformation_matrix() const noexcept
{
    const glm::mat4 translateMatrix = glm::translate(m_position);
    const glm::mat4 rotationMatrix = glm::toMat4(m_rotation);
    const glm::mat4 scaleMatrix = glm::scale(m_scale);
    return translateMatrix * rotationMatrix * scaleMatrix;
}
