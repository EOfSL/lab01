#include "world_layer.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>


const std::list<vertex>& world_layer::vertexes() const noexcept
{
    return m_vertexes;
}

const std::vector<polygon>& world_layer::polygons() const noexcept
{
    return m_polygons;
}

vertex* world_layer::add_vertex(const vertex& a_vertex) noexcept
{
    std::list<vertex>::const_iterator it = std::find(m_vertexes.cbegin(),
                                                     m_vertexes.cend(),
                                                     a_vertex);
    if (it == m_vertexes.end()) {
        m_vertexes.push_back(a_vertex);
        return &m_vertexes.back();
    }
    else
        return const_cast<vertex*>(it.operator->());
}

void world_layer::add_polygon(const polygon& a_polygon) noexcept
{
    m_polygons.push_back(a_polygon.get_non_vertex_copy());
    polygon& newp = m_polygons.back();

    for (size_t i = 0; i < a_polygon.m_vertexes.size(); ++i) {
        vertex* v = add_vertex(*a_polygon.m_vertexes[i]);
        newp.m_vertexes.push_back(v);
    }
}

void
world_layer::add_polygon(const std::initializer_list<vertex>& a_polygon)
noexcept
{
    if (a_polygon.size() < 3)
        return;

    m_polygons.push_back(polygon());
    polygon& newp = m_polygons.back();

    for (const vertex& i : a_polygon) {
        vertex* v = add_vertex(i);
        newp.m_vertexes.push_back(v);
    }
}

void world_layer::add_layer(const world_layer& a_layer) noexcept
{
    for (const polygon& p : a_layer.m_polygons)
        add_polygon(p);
}

void world_layer::clear() noexcept
{
    m_polygons.clear();
    m_vertexes.clear();
}

bool world_layer::is_empty() const noexcept
{
    return m_polygons.empty();
}

void world_layer::prepare_to_draw()
{
    if (m_polygons.empty())
        throw(std::invalid_argument("Empty polygons set"));

    for (polygon& p : m_polygons)
        p.prepare_to_draw();
}

void world_layer::draw(const camera& a_camera) const noexcept
{
    for (const polygon& p : m_polygons)
        p.draw(a_camera);
}

//bool world_layer::merge(polygon& a_first_polygon, const polygon& a_second_polygon)
//{
//    // !!!!!!!!!!!!!!!!

//    // Удалить второй полигон
//    m_polygons.erase(std::find(m_polygons.cbegin(),
//                               m_polygons.cend(),
//                               a_second_polygon));
//}

world_layer
world_layer::box(
        const vec3d& a_offset,
        float a_width,
        float a_height,
        float a_length,
        bool a_is_solid,
        const material* a_material_wall,
        const material* a_material_floor,
        const material* a_material_cellar)
{
    // Создание вершин:
    const vertex p000 = vec3d(0,       0,        0       );
    const vertex p001 = vec3d(0,       0,        a_length);
    const vertex p010 = vec3d(0,       a_height, 0       );
    const vertex p011 = vec3d(0,       a_height, a_length);
    const vertex p100 = vec3d(a_width, 0,        0       );
    const vertex p101 = vec3d(a_width, 0,        a_length);
    const vertex p110 = vec3d(a_width, a_height, 0       );
    const vertex p111 = vec3d(a_width, a_height, a_length);

    return quad(a_offset, p000, p001, p010, p011, p100, p101, p110, p111,
                a_is_solid,
                a_material_wall, a_material_floor, a_material_cellar);
}

world_layer
world_layer::quad(
        const vec3d& a_offset,
        const vertex& a_000,
        const vertex& a_001,
        const vertex& a_010,
        const vertex& a_011,
        const vertex& a_100,
        const vertex& a_101,
        const vertex& a_110,
        const vertex& a_111,
        bool a_is_solid,
        const material* a_material_wall,
        const material* a_material_floor,
        const material* a_material_cellar)
{
    world_layer b;

    // Создание граней
    /*  Перечесление точек происходит так: смотря на плоскость со стороны
     *  нормали, перечислим все точки против часовой стрелки (чтобы соблюдалось
     *  правило правой тройки).
     */

    // 11 -> 01 -> 00 -> 10

    // Южная xx0
    b.add_polygon({a_110 + a_offset, a_010 + a_offset, a_000 + a_offset, a_100 + a_offset});
    b.m_polygons.back().m_material = a_material_wall;
    // Северная xx1
    b.add_polygon({a_011 + a_offset, a_111 + a_offset, a_101 + a_offset, a_001 + a_offset});
    b.m_polygons.back().m_material = a_material_wall;
    // Западная 0xx
    b.add_polygon({a_010 + a_offset, a_011 + a_offset, a_001 + a_offset, a_000 + a_offset});
    b.m_polygons.back().m_material = a_material_wall;
    // Восточная 1xx
    b.add_polygon({a_111 + a_offset, a_110 + a_offset, a_100 + a_offset, a_101 + a_offset});
    b.m_polygons.back().m_material = a_material_wall;
    // Верхняя x1x
    b.add_polygon({a_111 + a_offset, a_011 + a_offset, a_010 + a_offset, a_110 + a_offset});
    b.m_polygons.back().m_material = a_material_cellar;
    // Нижняя x0x
    b.add_polygon({a_100 + a_offset, a_000 + a_offset, a_001 + a_offset, a_101 + a_offset});
    b.m_polygons.back().m_material = a_material_floor;

    if (a_is_solid)
        for (polygon& p : b.m_polygons)
            p.flip();

    if (!a_is_solid) {
        b.m_polygons[0].m_void = false;
        b.m_polygons[1].m_void = false;
        b.m_polygons[2].m_void = false;
        b.m_polygons[3].m_void = false;
        b.m_polygons[4].m_void = false;
        b.m_polygons[5].m_void = false;
    }

    return b;
}

world_layer::world_layer(const std::vector<vertex>& a_vec) noexcept
{
    std::copy(a_vec.cbegin(), a_vec.cend(), std::back_inserter(m_vertexes));
}

world_layer::world_layer() noexcept
{}

world_layer::world_layer(const polygon& a_polygon) noexcept
{
    add_polygon(a_polygon);
}

world_layer::world_layer(const std::vector<vertex*>& a_vec) noexcept
{
    std::transform(a_vec.cbegin(),
                   a_vec.cend(),
                   std::back_inserter(m_vertexes),
                   [](const vertex* v) {
        return *v;
    });
}

world_layer::world_layer(const world_layer& a) noexcept
{
    for (const polygon& p : a.m_polygons)
        add_polygon(p);
}

world_layer::world_layer(world_layer&& a) noexcept
    : m_vertexes(std::move(a.m_vertexes)), m_polygons(std::move(a.m_polygons))
{
    a.m_vertexes.clear();
    a.m_polygons.clear();
}

world_layer& world_layer::operator=(const world_layer& a) noexcept
{
    if (&a == this)
        return *this;

    m_vertexes = a.m_vertexes;
    m_polygons = a.m_polygons;
    return *this;
}

world_layer& world_layer::operator=(world_layer&& a) noexcept
{
    if (&a == this)
        return *this;

    m_vertexes = std::move(a.m_vertexes);
    m_polygons = std::move(a.m_polygons);
    a.m_vertexes.clear();
    a.m_polygons.clear();
    return *this;
}

world_layer::~world_layer() noexcept
{}
