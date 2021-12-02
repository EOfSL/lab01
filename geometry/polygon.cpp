#include "polygon.h"

#include <algorithm>
#include <stdexcept>
#include <glm/glm.hpp>
#include <iostream>


polygon::polygon(const polygon& a) noexcept
    : m_vertexes(a.m_vertexes), m_material(a.m_material), m_void(a.m_void)
{}

polygon::polygon(polygon&& a) noexcept
    : m_vertexes(std::move(a.m_vertexes)), m_material(a.m_material),
      m_void(a.m_void)
{
    a.m_material = nullptr;
    a.m_vertexes.clear();
}

polygon::polygon(const material* a_material) noexcept
    : m_material(a_material)
{}

polygon::polygon(const std::initializer_list<vertex*>& a_list, const material* a_material) noexcept
    : m_vertexes(a_list), m_material(a_material)
{}

polygon::~polygon() noexcept
{
    glDeleteVertexArrays(1, &m_vao);
}

polygon& polygon::operator=(const polygon& a) noexcept
{
    if (&a == this)
        return *this;

    m_vertexes = a.m_vertexes;
    m_material = a.m_material;
    m_void = a.m_void;

    return *this;
}

polygon& polygon::operator=(polygon&& a) noexcept
{
    if (&a == this)
        return *this;

    m_vertexes = std::move(a.m_vertexes);
    m_material = a.m_material;
    m_void = a.m_void;
    a.m_material = nullptr;
    a.m_vertexes.clear();

    return *this;
}

polygon polygon::get_non_vertex_copy() const noexcept
{
    polygon p;
    p.m_material = m_material;
    p.m_void = m_void;
    return p;
}

const vertex& polygon::any_vertex() const noexcept
{
    return *m_vertexes[0];
}

polygon::const_edge_iterator polygon::edges_cbegin() const noexcept
{
    return polygon::const_edge_iterator(m_vertexes, m_vertexes.cbegin());
}

polygon::const_edge_iterator polygon::edges_cend() const noexcept
{
    return polygon::const_edge_iterator(m_vertexes, m_vertexes.cend());
}

bool polygon::is_valid() const noexcept
{
    /* Проверка того что все точки грани лежат в одной плоскости: строится
     * нормаль плоскости первых трёх точек и сравнивается с нормалью точек
     * 1, 2 и i для всех i > 3.
     *
     * Одновременно проверяется и корректность порядка точек: они должны
     * формировать правую тройку. Если это не так, некоторые нормали будут
     * перевёрнуты.
     */

    if (m_vertexes.size() < 3)
        return false;

    const vec3d v12 = *m_vertexes[1] - *m_vertexes[0];
          vec3d v1i = *m_vertexes[2] - *m_vertexes[0];
    const vec3d normal = vec3d::cross_product(v12, v1i).normalized();

    for (unsigned i = 3; i < m_vertexes.size(); ++i) {
        v1i = *m_vertexes[i] - *m_vertexes[0];
        if (normal != vec3d::cross_product(v12, v1i).normalized())
            return false;
    }

    /* Проверка выпуклости грани производится !!!!!!!!!!1.
     *
     */

    return true;
}

//vec3d polyon::proection(const vec3d& a_p1, const vec3d& a_p2, const edge* a_edge) const noexcept
//{
//    auto it = edges_cbegin();
//    vec3d minp;
//    vertex minedge = it.first();
//    float mind = std::numeric_limits<float>::max();
//    while (it++ != edges_cend()) {
//        const edge e = *it;
//        float dist;
//        const vec3d tmp = vec3d::intersection(a_p1, a_p2, e.first, e.second, &dist);

//        if (dist < mind) {
//            mind = dist;
//            minp = tmp;
//            minedge = it.first();
//        }
//    }

//    if (a_edge)
//        *a_edge = edge(minedge, minedge + 1);

//    return minp;
//}

vec3d polygon::normal() const noexcept
{
    return normal_r().normalized();
}

vec3d polygon::normal_r() const noexcept
{
    const vec3d v12 = vec3d(*m_vertexes[1]) - vec3d(*m_vertexes[0]);
    const vec3d v13 = vec3d(*m_vertexes[2]) - vec3d(*m_vertexes[0]);

    return vec3d::cross_product(v12, v13);
}

void polygon::flip() noexcept
{
    std::reverse(m_vertexes.begin(), m_vertexes.end());
}

void polygon::prepare_to_draw() const
{
    if (m_vertexes.size() < 3)
        throw(std::invalid_argument("Draving polygon without vertexes"));

    if (!is_valid()) {
        print();
        throw(std::invalid_argument("Draving invalid polygon"));
    }

    if (!m_material)
        throw(std::invalid_argument("Polygon hase no material"));

    // Позиции вершин:
    std::vector<glm::vec3> positions;
    // UV вершин:
    std::vector<glm::vec2> uvs;

    // Добавить треугольники
    const vec3d u = (*m_vertexes[1] - *m_vertexes[0]).normalized();
    const vec3d v = vec3d::cross_product(u, normal_r()).normalized();
    for (unsigned i = 2; i < m_vertexes.size(); ++i) {
        // Заполнение координат вершин
        positions.push_back(m_vertexes[0]->to_vec3());
        positions.push_back(m_vertexes[i - 1]->to_vec3());
        positions.push_back(m_vertexes[i]->to_vec3());
        // Заполнение uv-координат
        uvs.push_back(glm::vec2(0, 0));
        const vec3d v1 = *m_vertexes[i - 1] - *m_vertexes[0];
        uvs.push_back(glm::vec2(u * v1, v * v1));
        const vec3d v2 = *m_vertexes[i] - *m_vertexes[0];
        uvs.push_back(glm::vec2(u * v2, v * v2));
    }

    // Число треугольников
    const_cast<GLuint&>(m_draw_count) = positions.size();

    // Удалить (один) массив если он там был
    glDeleteVertexArrays(1, &m_vao);

    // Создать (один) массив
    glGenVertexArrays(1, const_cast<GLuint*>(&m_vao));
    // Сказать использовать этот массив вертексов как текущий
    glBindVertexArray(m_vao);

    // Создать буфферы в гпу
    glGenBuffers(NUM_BUFFERS, const_cast<GLuint*>(m_vao_buffers));

    // ПОЗИЦИИ
    // Первый (POSITION_VB) из этих буфферов -- буффер позиций вертексов
    glBindBuffer(GL_ARRAY_BUFFER, m_vao_buffers[POSITION_VB]);
    // Записать данные в буфер (что именно для меня означает GL_STATIC_DRAW?)
    glBufferData(GL_ARRAY_BUFFER, m_draw_count * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
    // Сказать использовать первое поле в классе raw_vertex (m_pos)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UV
    // Второй (UV_VB) из этих буфферов -- буффер позиций вертексов
    glBindBuffer(GL_ARRAY_BUFFER, m_vao_buffers[UV_VB]);
    // Записать данные в буфер (что именно для меня означает GL_STATIC_DRAW?)
    glBufferData(GL_ARRAY_BUFFER, m_draw_count * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
    // Сказать использовать первое поле в классе raw_vertex (m_pos)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Прекретить использование массива вертексов
    glBindVertexArray(0);
}

void polygon::draw(const camera& a_camera) const noexcept
{
    // Использовать текстуру
    m_material->bind();
    m_material->update(a_camera);

    // Сказать использовать этот массив вертексов как текущий
    glBindVertexArray(m_vao);

    // Нарисовать m_draw_counts треугольников, начиная с первого (с 0),
    // то есть все.
    glDrawArrays(GL_TRIANGLES, 0, m_draw_count);

//    float* g = new float[m_draw_count * sizeof(glm::vec3)];
//    glBindBuffer(GL_ARRAY_BUFFER, m_vao_buffers[POSITION_VB]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_draw_count * sizeof(glm::vec3), g);
//    for (unsigned i = 0; i < m_draw_count * sizeof(glm::vec3); ++i)
//        std::cout << g[i] << ((i+1) % 3 ? " " : ", ");
//    std::cout << std::endl;
//    std::cout << m_draw_count << " " << (m_draw_count * sizeof(glm::vec3)) << std::endl;

    // Прекретить использование массива вертексов
    glBindVertexArray(0);
}

void polygon::print() const noexcept
{
    for (const vertex* v : m_vertexes)
        std::cout << v->m_x << " " << v->m_y << " " << v->m_z << std::endl;
}

polygon::const_edge_iterator& polygon::const_edge_iterator::operator++() noexcept
{
    ++m_ptr;
    return *this;
}

polygon::const_edge_iterator polygon::const_edge_iterator::operator++(int) noexcept
{
    auto it = *this;
    ++m_ptr;
    return it;
}

polygon::const_edge_iterator::value_type polygon::const_edge_iterator::operator*() const noexcept
{
    if (m_ptr + 1 == m_vec.end())
        return value_type(**m_ptr, **m_vec.begin());
    else
        return value_type(**m_ptr, **(m_ptr + 1));
}

const vertex& polygon::const_edge_iterator::first() const noexcept
{
    return **m_ptr;
}

const vertex& polygon::const_edge_iterator::second() const noexcept
{
    if (m_ptr + 1 == m_vec.end())
        return **m_vec.begin();
    else
        return **(m_ptr + 1);
}
