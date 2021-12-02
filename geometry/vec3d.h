#ifndef VEC3D_H
#define VEC3D_H

#include <math.h>
#include <array>
#include <glm/vec3.hpp>


class vec3d;

using vertex = vec3d;

class vec3d
{

public:

    constexpr vec3d() noexcept
        : m_x(0), m_y(0), m_z(0)
    {}

    constexpr vec3d(float a_x, float a_y, float a_z) noexcept
        : m_x(a_x), m_y(a_y), m_z(a_z)
    {}

    constexpr vec3d(const vec3d& a) noexcept
        : m_x(a.m_x), m_y(a.m_y), m_z(a.m_z)
    {}

    static constexpr vec3d zero() noexcept
    {;
        return vec3d(0, 0, 0);
    }

    constexpr glm::vec3 to_vec3() const noexcept
    {
        return glm::vec3(m_x, m_y, m_z);
    }

    constexpr bool is_valid() const noexcept
    {
        return std::isnan(m_x) || std::isnan(m_y) || std::isnan(m_z);
    }

    static vec3d nan() noexcept
    {
        return vec3d(std::nanf(""), std::nanf(""), std::nanf(""));
    }

    constexpr vec3d& operator=(const vec3d& a) noexcept
    {
        m_x = a.m_x; m_y = a.m_y; m_z = a.m_z;
        return *this;
    }

    constexpr float length() const noexcept
    {
        return std::sqrt(std::pow(m_x, 2) + std::pow(m_y, 2) + std::pow(m_z, 2));
    }

    constexpr vec3d normalized() const noexcept
    {
        float l = 1 / length();
        return vec3d(m_x * l, m_y * l, m_z * l);
    }

    constexpr vec3d operator-() noexcept
    {
        return vec3d(-m_x, -m_y, -m_z);
    }

    friend constexpr vec3d operator+(const vec3d& a, const vec3d& b) noexcept
    {
        return vec3d(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z);
    }

    friend constexpr vec3d operator-(const vec3d& a, const vec3d& b) noexcept
    {
        return vec3d(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
    }

    friend constexpr float operator*(const vec3d& a, const vec3d& b) noexcept
    {
        return (a.m_x *  b.m_x) + (a.m_y * b.m_y) + (a.m_z * b.m_z);
    }

    friend constexpr vec3d operator*(const vec3d& a, float b) noexcept
    {
        return vec3d(a.m_x *  b, a.m_y * b, a.m_z * b);
    }

    friend constexpr vec3d operator*(float b, const vec3d& a) noexcept
    {
        return vec3d(a.m_x *  b, a.m_y * b, a.m_z * b);
    }

    static constexpr float epsilon = 0.01;

    friend constexpr bool operator==(const vec3d& a, const vec3d& b) noexcept
    {
        return std::abs(a.m_x - b.m_x) < epsilon &&
               std::abs(a.m_y - b.m_y) < epsilon &&
               std::abs(a.m_z - b.m_z) < epsilon;
    }

    friend constexpr bool operator!=(const vec3d& a, const vec3d& b) noexcept
    {
        return std::abs(a.m_x - b.m_x) >= epsilon &&
               std::abs(a.m_y - b.m_y) >= epsilon &&
               std::abs(a.m_z - b.m_z) >= epsilon;
    }

    static constexpr vec3d cross_product(const vec3d& a, const vec3d& b) noexcept
    {
        return vec3d(a.m_y * b.m_z - a.m_z * b.m_y,
                     a.m_z * b.m_x - a.m_x * b.m_z,
                     a.m_x * b.m_y - a.m_y * b.m_x);
    }

    // DELETE
    static constexpr float rotate(const vec3d& a, const vec3d& b, const vec3d& c) noexcept
    {
        // Вычисление определителя.
        // Если его значение больше нуля, то поворот от вектора b - a до c - a
        // происходит против часовой стрелке, и по часовой стрелке иначе.
        float p = a.m_x * b.m_y * c.m_z +
                  a.m_y * b.m_z * c.m_x +
                  a.m_z * b.m_x * c.m_y;
        float m = a.m_z * b.m_y * c.m_x +
                  a.m_y * b.m_x * c.m_z +
                  a.m_x * b.m_z * c.m_y;
        return p - m;
    }

    float m_x, m_y, m_z;
};

#endif // VEC3D_H
