#include "world.h"

#include "compilers/csg_compiler.h"


world::world()
{

}

void world::add_brush(const brush& a_brush, const vec3d& a_offset) noexcept
{
    brush b = a_brush;
    std::for_each(b.m_raw_world_layer.m_vertexes.begin(),
                  b.m_raw_world_layer.m_vertexes.end(),
                  [&a_offset](vertex& v) {
        v = v + a_offset;
    });
    std::for_each(b.m_compiled_world_layer.m_vertexes.begin(),
                  b.m_compiled_world_layer.m_vertexes.end(),
                  [&a_offset](vertex& v) {
        v = v + a_offset;
    });
    m_brushes.push_back(std::move(b));
}

world_layer world::get_full_world_layer() const noexcept
{
    world_layer wl;
    for (const brush& b : m_brushes)
        wl.add_layer(b.m_compiled_world_layer);
    return wl;
}
