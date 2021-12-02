#ifndef CSG_COMPILER_H
#define CSG_COMPILER_H

#include <vector>

#include "geometry/world.h"


class csg_compiler
{
public:
    csg_compiler() noexcept;

    // Результат компиляции каждого браша лежит у него самого в виде
    // world_layer. Так сделано (а не вернуть целый world_layer всего мира)
    // для возможности перекомпилировать отдельные браши без изменения
    // остального мира. Потому ссылка на world и не константая.
    void compile(world&) const noexcept;

    // Перекомпилировать конкретный браш
    void recompile_brush(world&, brush&) const noexcept;
    void recompile_brush(world&, std::vector<brush>::iterator) const noexcept;

protected:

    // Проверка наличия пересечения двух брашей
    static bool are_brushes_intersecting(const brush&, const brush&) noexcept;

    // Перекомпилировать браш с учётом второго браша
    static void recompile_brush_by_brush(brush&, const brush&, bool) noexcept;

    // Перекомпилировать полигон с учётом второго браша
    static void recompile_polygon_by_brush(world_layer&, const brush&, const brush&, bool) noexcept;

    // Перекомпилировать полигон с учётом второго полигона
    static void split_polygon(world_layer&, std::vector<polygon>::iterator, const polygon&) noexcept;
};

#endif // CSG_COMPILER_H
