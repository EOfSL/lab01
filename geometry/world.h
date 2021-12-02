#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "brush.h"
#include "world_layer.h"


class world
{
public:

    std::vector<brush> m_brushes;

    world();

    void add_brush(const brush&, const vec3d& = vec3d::zero()) noexcept;

    // Получить объединение слоёв всех брашей
    world_layer get_full_world_layer() const noexcept;
};

#endif // WORLD_H
