#include "bsp_tree.h"

#include <algorithm>
#include <vector>

#include "geometry_helper.h"


bsp_tree::bsp_node* bsp_tree::root() noexcept
{
    return &m_root;
}

bsp_tree::bsp_tree() noexcept
    : m_root(bsp_node(nullptr))
{}

bsp_tree::~bsp_tree() noexcept
{
    delete m_root.m_front;
    delete m_root.m_back;
}

std::vector<polygon>::const_iterator
bsp_tree::default_splitter_scorer(
        const world_layer&
        a_world_layer)
noexcept
{
    return a_world_layer.polygons().cbegin();
}

void
bsp_tree::build_bsp_tree(
        bsp_node* a_node,
        const world_layer& a_world_layer,
        splitter_scorer a_splitter_acorer)
{
    // Получение сплиттера
    const polygon& splitter = *a_splitter_acorer(a_world_layer);

    // Добавить в ноду дерева сплиттер и все грани, находящиеся со сплиттером
    // в одной плоскости:
    const world_layer coplonarFaces = geometry_helper::get_world_on_side_of(a_world_layer, splitter, geometry_helper::side::coplonar);
    for (const polygon& i : coplonarFaces.polygons())
        a_node->add_face(i);

    // Два массива для полигонов перед сплиттером и за ним.
    const world_layer frontFaces = geometry_helper::get_world_on_side_of(a_world_layer, splitter, geometry_helper::side::front);
    if (!frontFaces.polygons().empty()) {
        a_node->m_front = new bsp_node(a_node);
        build_bsp_tree(a_node->m_front, frontFaces, a_splitter_acorer);
    }

    const world_layer backFaces = geometry_helper::get_world_on_side_of(a_world_layer, splitter, geometry_helper::side::back);
    if (!backFaces.polygons().empty()) {
        a_node->m_back = new bsp_node(a_node);
        build_bsp_tree(a_node->m_back, backFaces, a_splitter_acorer);
    }
}

bsp_tree*
bsp_tree::compile(
        const world_layer& a_world_layer,
        splitter_scorer a_splitter_acorer)
{
    bsp_tree* tree = new bsp_tree;
    build_bsp_tree(tree->root(), a_world_layer, a_splitter_acorer);
    return tree;
}

bsp_tree::bsp_node::bsp_node(bsp_node* a_parent) noexcept
    : m_parent(a_parent)
{}

bsp_tree::bsp_node::~bsp_node() noexcept
{
    delete m_front;
    delete m_back;
}

void bsp_tree::bsp_node::add_face(const polygon&) noexcept
{
    // !!!!!!!!!!!
}

