#include "csg_compiler.h"

#include <limits>
#include <iostream>

#include "geometry/geometry_helper.h"


csg_compiler::csg_compiler() noexcept
{}

void csg_compiler::compile(world& a_world) const noexcept
{
    auto it = a_world.m_brushes.begin();
    while (it != a_world.m_brushes.end()) {
        recompile_brush(a_world, it);
        ++it;
    }
}

void
csg_compiler::recompile_brush(
        world& a_world,
        brush& a_brush)
const noexcept
{
    auto it = a_world.m_brushes.begin();
    while (it != a_world.m_brushes.end()) {
        if (it.operator->() == &a_brush) {
            recompile_brush(a_world, it);
            break;
        }
        ++it;
    }
}

void
csg_compiler::recompile_brush(
        world& a_world,
        std::vector<brush>::iterator a_brush)
const noexcept
{
    // По поводу смысла ib_overrider см. get_polygon_type().
    bool ib_override = false;

    auto it = a_world.m_brushes.begin();
    while (it != a_world.m_brushes.end()) {
        if (a_brush == it)
            ib_override = true;
        else
            recompile_brush_by_brush(*a_brush, *it, ib_override);
        ++it;
    }

    // Последний шаг -- удалить полигоны, в силу некоторых причин смотрящих
    // в пустоту. Не отсеиваются на предыдущих шагах они потому что алгоритм
    // выше занимается определением полигонов только в свободном пространстве,
    // а в данной модели "пустота" вне уровня -- заполнена.
    // Определяются они легко -- у них m_void = true.
    std::vector<polygon>& ps = a_brush->m_compiled_world_layer.m_polygons;
    ps.erase(std::remove_if(ps.begin(), ps.end(), [](const polygon& p) {
                                                      return p.m_void;
                                                  }), ps.cend());
}

void csg_compiler::recompile_brush_by_brush(
        brush& a_target_brush,
        const brush& a_influencing_brush,
        bool a_ib_override)
noexcept
{
    // Новый слой для скомпилированных полигонов
    world_layer new_wl;

    // Цикл по полигонам target_layer
    const std::vector<polygon>& target_polygons =
            a_target_brush.m_compiled_world_layer.m_polygons;
    for (const polygon& target_polygon : target_polygons) {
        world_layer p_wl(target_polygon);
        recompile_polygon_by_brush(p_wl,
                                   a_target_brush,
                                   a_influencing_brush,
                                   a_ib_override);
        new_wl.add_layer(p_wl);
    }

    a_target_brush.m_compiled_world_layer = std::move(new_wl);
}

enum class polygon_type
{
    to_delete, to_pass, to_cross
};
polygon_type
get_polygon_type(
        polygon& a_tp,
        const brush& a_target_brush,
        const brush& a_influencing_brush,
        const polygon** a_last_splitter,
        bool a_ib_override)
{
    /* Вне зависимости от своего типа, a_influencing_brush воздействует только
     * и только на ту геометрию что у него внутри, а внешнюю игнорирует.
     *
     * Вне зависимости от своего типа, a_influencing_brush удаляет любую
     * геометрию внутри себя только если a_ib_override равен true.
     */

    const std::vector<polygon>& influencing_polugons =
            a_influencing_brush.m_raw_world_layer.m_polygons;
    const char number_of_vertexes = a_tp.m_vertexes.size();

    bool ctd = false;

    for (const polygon& p : influencing_polugons) {

        unsigned char i_front = 0, i_back = 0, i_on_plane = 0;
        for (const vertex* v : a_tp.m_vertexes) {
            float d = geometry_helper::distance(p, *v);

            // Логика функции не должна менятся от типа a_influencing_brush,
            // однако у полого a_influencing_brush нужно "развернуть нормали"
            // чтобы посчитать расстояние правильно.
            if (!a_influencing_brush.m_is_solid)
                d *= -1;

            if (d > vec3d::epsilon)
                ++i_front;
            else if (d < -vec3d::epsilon)
                ++i_back;
            else
                ++i_on_plane;
        }

        // a_tp копланарен p:
        if (i_on_plane == number_of_vertexes) {
            if ((p.normal() == a_tp.normal()) && !a_ib_override)
                return polygon_type::to_pass;
            else if (a_target_brush.m_is_solid)
                ctd = true;
        }
        // a_tp находится перед p (снаружи шейпа браша):
        else if (i_front + i_on_plane == number_of_vertexes
                 && i_on_plane <= 2)
            return polygon_type::to_pass;
        // a_tp находится сзади p (внутри шейпа браша):
        else if (i_back + i_on_plane == number_of_vertexes
                 && i_on_plane <= 2);
        // Иначе a_tp пересекает плоскость p:
        else
            *a_last_splitter = &p;
    }

    if (ctd && !a_ib_override)
        return polygon_type::to_delete;

    // Здесь указатель может быть равен нулю только в одном случае -- a_tp
    // внутри браша.
    if (*a_last_splitter == nullptr) {
        if (!a_influencing_brush.m_is_solid &&
                a_target_brush.m_is_solid &&
                !a_ib_override) {
            // Гарантия что полигон смотрит в пустое пространство
            a_tp.m_void = false;
            return polygon_type::to_pass;
        }
        return polygon_type::to_delete;
    }

    return polygon_type::to_cross;
}

void
csg_compiler::recompile_polygon_by_brush(
        world_layer& a_target_polygon_layer,
        const brush& a_target_brush,
        const brush& a_influencing_brush,
        bool a_ib_override)
noexcept
{
    // План:
    // 1. Попробовать отсеять a_target_polygon_layer, удалить или пропустить,
    // пробежав по полигонам браша и вычислив положение a_target_polygon_layer
    // относительно его.
    // 2. Перебрать полигоны a_influencing_brush и найти те что пересекают
    // a_target_polygon_layer.

    size_t current_polygon_i = 0;
    std::vector<polygon>& target_polygons = a_target_polygon_layer.m_polygons;
    size_t L = a_target_polygon_layer.m_polygons.size();
    while (current_polygon_i != L) {

        polygon& current_polygon = target_polygons[current_polygon_i];

        const polygon* splitter = nullptr;
        const polygon_type type = get_polygon_type(current_polygon,
                                                   a_target_brush,
                                                   a_influencing_brush,
                                                   &splitter,
                                                   a_ib_override);

        switch (type) {
        case polygon_type::to_delete:
            target_polygons.erase(target_polygons.cbegin() + current_polygon_i);
            return;
        case polygon_type::to_pass:
            ++current_polygon_i;
            continue;
        case polygon_type::to_cross:
            geometry_helper::split_by_a_plane(a_target_polygon_layer,
                                              current_polygon_i,
                                              *splitter);
            ++L;
            continue;
        }
    }
}

//            static const material MAT("./textures/bricks.png");
//            polygon& ppp = const_cast<polygon&>(current_polygon);
//            ppp.m_material = &MAT;
