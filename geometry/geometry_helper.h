#ifndef GEOMETRY_HELPER_H
#define GEOMETRY_HELPER_H

#include "world_layer.h"


namespace geometry_helper
{

enum class side {front, back, coplonar};


bool is_parallel(const polygon& a, const polygon& b) noexcept;

// Проверка находится ли точка внутри ВЫПУКЛОЙ грани
// DELETE
bool is_point_in_convex_polygon(const polygon& a_polyon, const vec3d& a_point) noexcept;


// Проверка на коллениарность двух векторов
bool is_colleniar(const vec3d&, const vec3d&) noexcept;


// Получить расстояние от точки до плоскости грани
float distance(const polygon& a_polyon, const vec3d& a_point) noexcept;


// Получить расположение точки относительно плоскости грани
side get_relative_side_of_point(const polygon& a_polyon, const vec3d& a_point) noexcept;


// Возвращает множитель, который определяет точку пересечения прямой a_edge
// и плоскости a_plane. Случай когда a_edge не пересекается с плоскостью a_plane
// или лежит в нём не рассматривается -- напишет true в a_er.
float split_by_a_plane(const vec3d&, const vec3d&, const polygon& a_plane, bool* a_er) noexcept;

// Берёт a_i-ю грань в списке в a_target и делит её плоскостью a_plane.
// На место a_i-го полигона вставляется тот кусок что расположен со стороны
// нормали a_plane, второй кусок добавляется к списку с конца.
// Если входные полигоны не пересекаются -- поведение неопределено.
void split_by_a_plane(world_layer& a_target, const size_t a_i, const polygon& a_plane) noexcept;


//std::pair<polyon, polyon> split_by_a_polyon(const polyon& a_target, const polyon& a_polyon)
//{
//    vec3d vec = vec3d::cross_product(a_target.normal(), a_polyon.normal());



//}


// Поиск пересечения прямых (a1-a2) и (b1-b2), расположенных на
// одной плоскости и не параллельных. Иной случай не рассматривается.
// Также пишет в dist расстояние вдоль (a1-a2) от a1 до точки пересеченя.
vec3d intersection(const vec3d& a1, const vec3d& a2, const vec3d& b1, const vec3d& b2, float* dist = nullptr) noexcept;


// Отсечь набор полигонов плоскостью заданной грани
world_layer get_world_on_side_of(const world_layer& a_polyonsList, const polygon& a_splitter, side a_side) noexcept;

}

#endif // GEOMETRY_HELPER_H
