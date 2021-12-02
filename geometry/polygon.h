#ifndef polyon_H
#define polyon_H

#include <algorithm>
#include <vector>

#include "vec3d.h"
#include "render/material.h"


/**
 * @brief Класс полигона. Состоит из указателей на вершины, из которых состоит,
 * и материал. Умеем рисовать сам себя. Обязан быть выпуклым. Всегда связный.
 */
class polygon
{
public:

    // Вершины грани. Нормаль направлена по правилу правой тройки, то есть
    // если камера смотрит против нормали, то вершины перечисляются против
    // часовой стрелки.
    std::vector<vertex*> m_vertexes;
    const material* m_material = nullptr;
    // Смотрит ли полигон в пустоту
    bool m_void = true;

    // ==========================
    // Данные отрисовки
protected:
    static const unsigned POSITION_VB = 0;
    static const unsigned UV_VB = 1;
    static const unsigned NUM_BUFFERS = 2;
    // Пространство для массива вертексов
    GLuint m_vao = 0;
    // Массив указателей на буфферы в гпу
    GLuint m_vao_buffers[NUM_BUFFERS];
    GLuint m_draw_count = 0;

public:
    polygon(const polygon&) noexcept;
    polygon(polygon&&) noexcept;
    explicit polygon(const material* = nullptr) noexcept;
    explicit polygon(const std::initializer_list<vertex*>&, const material*) noexcept;
    ~polygon() noexcept;
    polygon& operator=(const polygon&) noexcept;
    polygon& operator=(polygon&&) noexcept;

    polygon get_non_vertex_copy() const noexcept;

    const vertex& any_vertex() const noexcept;

    using edge = std::pair<const vertex&, const vertex&>;

    // (Не циклический) итератор по рёбрам
    struct const_edge_iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = const edge;
        using container_type    = const std::vector<vertex*>;

        const_edge_iterator(container_type& a_vec, container_type::const_iterator a_ptr)
            : m_vec(a_vec), m_ptr(a_ptr) {}

        const_edge_iterator(const const_edge_iterator&) = default;

        value_type operator*() const noexcept;
        const vertex& first() const noexcept;
        const vertex& second() const noexcept;
        const_edge_iterator& operator++() noexcept;
        friend const_edge_iterator operator+(const const_edge_iterator& a, int d) noexcept
        { return const_edge_iterator(a.m_vec, a.m_ptr + d); }
        const_edge_iterator operator++(int) noexcept;
        friend bool operator==(const const_edge_iterator& a, const const_edge_iterator& b) noexcept
        { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const const_edge_iterator& a, const const_edge_iterator& b) noexcept
        { return a.m_ptr != b.m_ptr; }

    private:
        container_type& m_vec;
        container_type::const_iterator m_ptr;
    };

    const_edge_iterator edges_cbegin() const noexcept;
    const_edge_iterator edges_cend() const noexcept;

    // Проверка плоскости
    bool is_valid() const noexcept;

    // Даны две точки, найти ближайшее к первой точке пересечение прямой
    // между двумя точками и данной гранью. По указателю возвращает и ребро.
    vec3d proection(const vec3d&, const vec3d&, const edge* = nullptr) const noexcept;

    // Получить нормаль
    vec3d normal() const noexcept;
    vec3d normal_r() const noexcept;

    // Развернуть полигон (нормаль)
    void flip() noexcept;

    void prepare_to_draw() const;
    void draw(const camera&) const noexcept;

    void print() const noexcept;
};

#endif // polyon_H
