#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>


class transform
{
public:

    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    transform(const glm::vec3& = glm::vec3(), const glm::quat = glm::quat(), const glm::vec3& = glm::vec3(1.0f, 1.0f, 1.0f)) noexcept;

    void translate(glm::vec3) noexcept;
    void rotate(float, glm::vec3) noexcept;
    void scaleBy(glm::vec3) noexcept;

    glm::mat4 calculate_transformation_matrix() const noexcept;
};

#endif // TRANSFORM_H
