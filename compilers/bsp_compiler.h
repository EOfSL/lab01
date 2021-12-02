#ifndef BSP_COMPILER_H
#define BSP_COMPILER_H

#include "geometry/world_layer.h"


struct bsp_node
{
    bsp_node* m_parent = nullptr;
    bsp_node* m_front = nullptr;
    bsp_node* m_back = nullptr;
    world_layer m_leaf_wl;
    bool m_is_leaf = false;

    explicit bsp_node(bsp_node*, const world_layer&) noexcept;
    ~bsp_node() noexcept;

    void render(const camera&) const noexcept;
};

class bsp_compiler
{
public:
    bsp_compiler();

    bsp_node* compile(const world_layer&) const noexcept;
};

#endif // BSP_COMPILER_H
