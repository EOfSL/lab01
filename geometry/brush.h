#ifndef BRUSH_H
#define BRUSH_H

#include <stdexcept>

#include "world_layer.h"


class world;

/**
 * @brief Объект браша состоит из двух наборов вершин и граней:
 * 1. Исходного, формирующего независимую форму браша. Должен представлять
 * собой выпуклый многогранник;
 * 2. Формирующую форму браша после его компиляции, которая используется для
 * отрисовки. В частности, в векторе граней лежат именно полигоны.
 *
 * Исходная форма обязана быть выпуклой, полигоны соответственно тоже. После
 * компиляции не обязан, как и его полигоны.
 *
 * При рекомпиляции браша он ищет пересечения с остальными брашами.
 */
class brush
{
public:

    world_layer m_raw_world_layer;
    world_layer m_compiled_world_layer;
    // Проще хранить эти числа чем перерассчитывать каждый раз
    float m_minX = 0, m_maxX = 0;
    float m_minY = 0, m_maxY = 0;
    float m_minZ = 0, m_maxZ = 0;


    // true -- браш вырезает пространство, false -- заполняет.
    // Нормали граней по-умолчанию смотрят во внутрь, потому следует
    // разворачивать их если m_isExcise = false.
    bool m_is_solid = true;

    brush() noexcept;
    brush(const world_layer&) noexcept;

    // Проверка браша
    bool is_valid() const noexcept;

    // Копирование сырого состояния в скомпилированное
    void reset() noexcept;

    /* Проверка положения точки относительно (исходной формы) браша. В общем
     * случае функция проверяет лежит ли точка во "внешнем" пространстве браша:
     * для закрытых брашей вернёт true если точка находится внутри, а для
     * открытых вернёт true если точка находится снаружи, и наоборот.
     *
     * Иначе говоря, false если точка находится в том участке пространства куда
     * смотрят все нормали полигонов браша.
     */
    bool is_point_in(const vec3d&) const noexcept;



    static brush box(const vec3d& a_offset,
                     float a_w,
                     float a_h,
                     float a_l,
                     bool a_is_solid,
                     const material* a_material_wall,
                     const material* a_material_floor,
                     const material* a_material_cellar)  noexcept;
    static brush quad(const vec3d& a_offset,
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
                      const material* a_material_cellar)  noexcept;
    static brush box(const vec3d& a_offset,
                     float a_w,
                     float a_h,
                     float a_l,
                     bool a_is_solid,
                     const material* a_material)  noexcept;
    static brush quad(const vec3d& a_offset,
                      const vertex& a_000,
                      const vertex& a_001,
                      const vertex& a_010,
                      const vertex& a_011,
                      const vertex& a_100,
                      const vertex& a_101,
                      const vertex& a_110,
                      const vertex& a_111,
                      bool a_is_solid,
                      const material* a_material)  noexcept;

private:
    void setup_minmax() noexcept;
};

#endif // BRUSH_H
