#include "geometry_helper.h"


bool geometry_helper::is_parallel(const polygon& a, const polygon& b) noexcept
{
    const vec3d an = a.normal();
    const vec3d bn = b.normal();
    return an == bn || an - bn == vec3d(0, 0, 0);
}

// DELETE
bool
geometry_helper::is_point_in_convex_polygon(
        const polygon& a_polygon,
        const vec3d& a_point)
noexcept
{
    // Точка должна хотя-бы лежать в плоскости a_polygon
    if (distance(a_polygon, a_point) != 0)
        return false;

    // Если всё-таки лежит
    auto it = a_polygon.edges_cbegin();
    while (it++ != a_polygon.edges_cend()) {
        const std::pair<const vertex&, const vertex&> pair = *it;
        if (vec3d::rotate(pair.first, pair.second, a_point) > 0)
            return false;
    }
    return true;
}

float
geometry_helper::distance(
        const polygon& a_polyon,
        const vec3d& a_point)
noexcept
{
    return a_polyon.normal() * (a_point - a_polyon.any_vertex());
}

geometry_helper::side
geometry_helper::get_relative_side_of_point(
        const polygon& a_polyon,
        const vec3d& a_point)
noexcept
{
    const float result = distance(a_polyon, a_point);

    if (result == 0) // Эксилон-окрестность сегодня не нужна же?
        return side::coplonar;
    else if (result > 0)
        return side::front;
    else
        return side::back;
}
#include <iostream>
float
geometry_helper::split_by_a_plane(
        const vec3d& a_v1,
        const vec3d& a_v2,
        const polygon& a_plane,
        bool* a_er)
noexcept
{
    // Поиск расстояния d от a_v1 до плоскости вдоль нормали:
    const vec3d  normal = a_plane.normal_r();
    const vec3d& any_vertex_on_plane = a_plane.any_vertex();
    float d1 = normal * (a_v1 - any_vertex_on_plane);
    float d2 = normal * (a_v2 - any_vertex_on_plane);
    if (d1 * d2 > 0) {
        *a_er = true;
        return std::nanf("");
    }
    d1 = std::abs(d1);
    d2 = std::abs(d2);

    return d1 / (d1 + d2);
    // То есть точка пересечения = a_v1 + (a_v2 - a_v1)*(d1 / (d1 + d2))
}

void
geometry_helper::split_by_a_plane(
        world_layer& a_target,
        const size_t a_i,
        const polygon& a_plane)
noexcept
{
    // cutA и cutB -- точки новой разделительной грани.
    // cutA и cutB будут стоять согласно порядку вершин a_target.
    polygon& origin_target_polygon = a_target.m_polygons[a_i];
    const std::vector<vertex*> vertexes = origin_target_polygon.m_vertexes;
    const size_t L = vertexes.size();

    vertex* cutA;
    vertex* cutB;
    size_t edgeA = 0;
    size_t edgeB = 0;
    size_t edgeAnext = 0;
    size_t edgeBnext = 0;
    bool flag = false;

    // Цикл по рёбрам a_target
    for (size_t i = 0; i < L; ++i) {

        const size_t next_i = i + 1 == L ? 0 : i + 1;
        const vertex* first = vertexes[i];
        const vertex* second = vertexes[next_i];
        bool error = false;

        const float l = split_by_a_plane(*first,
                                         *second,
                                         a_plane,
                                         &error);
        if (error)
            continue;

        const vec3d new_vertex = (*second - *first)*l + *first;

        if (!flag) {
            cutA = a_target.add_vertex(new_vertex);
            edgeA = i;
            edgeAnext = next_i;
        }
        else {
            cutB = a_target.add_vertex(new_vertex);
            edgeB = i;
            edgeBnext = next_i;
        }

        // Случай l = 0 (l = 1 не может быть):
        if (l == 0)
            --(!flag ? edgeA : edgeB);

        if (!flag)
            flag = true;
        else
            break;
    }

    // Имеем в cutA и cutB вершины сечения, а в edgeA и edgeB -- первые вершины
    // граней на которых лежат эти точки.
    // Теперь необходимо сформировать два новых полигона.

    // Первая новая грань:
    polygon new_poly1 = origin_target_polygon.get_non_vertex_copy();
    new_poly1.m_vertexes.push_back(cutA);
    new_poly1.m_vertexes.push_back(cutB);
    for (size_t i = edgeBnext; i != edgeAnext;) {
        new_poly1.m_vertexes.push_back(origin_target_polygon.m_vertexes[i]);
        if (i + 1 == L)
            i = 0;
        else
            ++i;
    }

    // Вторая новая грань:
    polygon new_poly2 = a_target.m_polygons.back().get_non_vertex_copy();
    new_poly2.m_vertexes.push_back(cutB);
    new_poly2.m_vertexes.push_back(cutA);
    for (size_t i = edgeAnext; i != edgeBnext;) {
        new_poly2.m_vertexes.push_back(origin_target_polygon.m_vertexes[i]);
        if (i + 1 == L)
            i = 0;
        else
            ++i;
    }

    origin_target_polygon = std::move(new_poly1);
    a_target.m_polygons.push_back(std::move(new_poly2));
}

world_layer
geometry_helper::get_world_on_side_of(
        const world_layer& a_world_layer,
        const polygon& a_splitter,
        side a_side)
noexcept
{
    world_layer res;

    // Необходимо проверить расположение всех вершин каждой грани из
    // a_polyonsList относительно a_polyon.
    for (const polygon& curpolyon : a_world_layer.polygons()) {

        bool was_front = false;
        bool was_back = false;

        for (const vertex* v : curpolyon.m_vertexes) {
            const side s = get_relative_side_of_point(a_splitter, *v);

            switch (s) {
                case side::front: was_front = true; break;
                case side::back: was_back = true; break;
                default:;
            }
        }

        if (was_front && was_back) {// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//            std::pair<polygon, polygon> r = split_by_a_plane(curpolyon, a_splitter);

//            const side ss = get_relative_side_of_point(a_splitter, *r.first.m_vertexes[2]);
//            if (ss == a_side)
//                res.add_polygon(r.first);
//            else
//                res.add_polygon(r.second);
        }
        else if ((a_side == side::front && !was_back)
                 || (a_side == side::back && !was_front)
                 || (a_side == side::coplonar))
            res.add_polygon(curpolyon);
    }

    return res;
}

bool geometry_helper::is_colleniar(const vec3d& a, const vec3d& b) noexcept
{
    return vec3d::cross_product(a, b) * vec3d(1, 1, 1) == 0;
}

vec3d
geometry_helper::intersection(
        const vec3d& a_a1,
        const vec3d& a_a2,
        const vec3d& a_b1,
        const vec3d& a_b2,
        float* a_dist)
noexcept
{
    // Известно, что прямые точно пересекаются, а значит достаточно решить
    // задачу в проекции на одну из осей:

    const float dax = a_a2.m_x - a_a1.m_x;
    const float dbx = a_b2.m_x - a_b1.m_x;
    const float day = a_a2.m_y - a_a1.m_y;
    const float dby = a_b2.m_y - a_b1.m_y;
    const float ux = a_a1.m_x - a_b1.m_x;
    const float uy = a_a1.m_y - a_b1.m_y;

    const float dist = (ux*dby - uy*dbx) / (day*dbx - dax*dby);

    const vec3d v = dist * (a_a2 - a_a1);
    if (a_dist)
        *a_dist = v.length();
    return v + a_a1;
}
