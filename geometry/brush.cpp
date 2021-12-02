#include "brush.h"

//#include "world.h"



brush::brush() noexcept
{}

brush::brush(const world_layer& a_world_layer) noexcept
    : m_raw_world_layer(a_world_layer), m_compiled_world_layer(a_world_layer)
{
//    setup_minmax();
}

bool brush::is_valid() const noexcept
{
    return true;
}

void brush::reset() noexcept
{
    m_compiled_world_layer = m_raw_world_layer;
}

bool brush::is_point_in(const vec3d& a_point) const noexcept
{
    /* Пользуясь требованием браша (его исходной формы) быть выпуклым,
     * достаточно проверить что a_point находится на стороне нормали для каждой
     * грани: вычесть из a_point любую вершину данной грани и проверить знак
     * косинуса между этой разницей и нормалью грани.
     *
     * Использование m_rawFaces вместо m_compiledFaces разумнее.
     */

    for (const polygon& f : m_raw_world_layer.m_polygons) {
        float prod = (a_point - f.any_vertex()) * f.normal();
        // Знак < позволяет учитывать точки _на_ грани как пересечение
        if ((prod * a_point.length()) >= 0)
            return false;
    }

    return true;
}

brush
brush::box(
        const vec3d& a_offset,
        float a_w,
        float a_h,
        float a_l,
        bool a_is_solid,
        const material* a_material_wall,
        const material* a_material_floor,
        const material* a_material_cellar)
noexcept
{
    brush b(world_layer::box(a_offset, a_w, a_h, a_l, a_is_solid,
                             a_material_wall, a_material_floor, a_material_cellar));
    b.m_is_solid = a_is_solid;
    return b;
}

brush
brush::quad(const vec3d& a_offset,
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
noexcept
{
    brush b(world_layer::quad(a_offset, a_000, a_001, a_010, a_011, a_100,
                              a_101, a_110, a_111, a_is_solid,
                              a_material_wall, a_material_floor, a_material_cellar));
    b.m_is_solid = a_is_solid;
    return b;
}

brush
brush::box(
        const vec3d& a_offset,
        float a_w,
        float a_h,
        float a_l,
        bool a_is_solid,
        const material* a_material)
noexcept
{
    brush b(world_layer::box(a_offset, a_w, a_h, a_l, a_is_solid,
                             a_material, a_material, a_material));
    b.m_is_solid = a_is_solid;
    return b;
}

brush
brush::quad(const vec3d& a_offset,
            const vertex& a_000,
            const vertex& a_001,
            const vertex& a_010,
            const vertex& a_011,
            const vertex& a_100,
            const vertex& a_101,
            const vertex& a_110,
            const vertex& a_111,
            bool a_is_solid,
            const material* a_material)
noexcept
{
    brush b(world_layer::quad(a_offset, a_000, a_001, a_010, a_011, a_100,
                              a_101, a_110, a_111, a_is_solid,
                              a_material, a_material, a_material));
    b.m_is_solid = a_is_solid;
    return b;
}

void brush::setup_minmax() noexcept
{
    const std::list<vertex>& vs = m_raw_world_layer.m_vertexes;
    std::list<vertex>::const_iterator res;

    res = std::max(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_x < w->m_x;
    });
    m_maxX = res->m_x;

    res = std::max(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_y < w->m_y;
    });
    m_maxY = res->m_y;

    res = std::max(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_z < w->m_z;
    });
    m_maxZ = res->m_z;

    res = std::min(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_x < w->m_x;
    });
    m_minX = res->m_x;

    res = std::min(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_y < w->m_y;
    });
    m_minY = res->m_y;

    res = std::min(vs.cbegin(),
                      vs.cend(),
                      [](const auto& v, const auto& w) {
        return v->m_z < w->m_z;
    });
    m_minZ = res->m_z;
}


