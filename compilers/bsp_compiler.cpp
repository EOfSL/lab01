#include "bsp_compiler.h"
#include "geometry/geometry_helper.h"

#include <iostream>


bsp_compiler::bsp_compiler()
{}

std::vector<polygon>::const_iterator
default_splitter_scorer(
        const world_layer&
        a_world_layer)
noexcept
{
    return a_world_layer.polygons().cbegin();
}


void
build_bsp_tree(bsp_node* a_node)
{
    std::cout << "Обработка " << a_node << std::endl;
    // =========================================================================
    // Условие выхода из рекурсии -- выпуклость a_node->m_leaf_wl.
    for (const polygon& p : a_node->m_leaf_wl.m_polygons) {
        for (const polygon& p1 : a_node->m_leaf_wl.m_polygons) {
            if (&p1 == &p)
                continue;
            if (std::any_of(p1.m_vertexes.cbegin(),
                            p1.m_vertexes.cend(),
                            [&p](const vertex* v) {
                                return geometry_helper::distance(p, *v) < 0;
                            }))
                goto p1;
        }
    }
    std::cout << "Лист" << std::endl;
    a_node->m_is_leaf = true;
    // Приготовить полигоны к отрисовке
    a_node->m_leaf_wl.prepare_to_draw();
    return;

    p1:;
    std::cout << "Не лист" << std::endl;

    const world_layer origin_wl = a_node->m_leaf_wl;

    // =========================================================================
    // Определение сплиттера

    auto it = std::find_if(origin_wl.m_polygons.cbegin(),
                           origin_wl.m_polygons.cend(),
                           [](const polygon& p) {
        return p.m_void != true;
    });

    if (it == origin_wl.m_polygons.cend()) {
        std::cout << "Лист 2" << std::endl;
        a_node->m_is_leaf = true;
        // Приготовить полигоны к отрисовке
        a_node->m_leaf_wl.prepare_to_draw();
        return;
    }

    const polygon& splitter = *it;
    const_cast<polygon&>(splitter).m_void = true;

    // =========================================================================
    world_layer front_wl;
    world_layer back_wl;

    // Необходимо пройтись по всем остальным полигонам в origin_wl и
    // распределить их на три группы: перед сплиттером, за ним и на его
    // плоскости. Первые две группы перейдут соответствующим потомкам a_node.
    for (const polygon& p : origin_wl.m_polygons) {

        // Определить положение полигона p относительно плоскости spllitter:
        unsigned char i_front = 0, i_back = 0, i_on_plane = 0;
        const unsigned char number_of_vertexes = splitter.m_vertexes.size();
        for (const vertex* v : p.m_vertexes) {
            float d = geometry_helper::distance(splitter, *v);

            if (d > vec3d::epsilon)
                ++i_front;
            else if (d < -vec3d::epsilon)
                ++i_back;
            else
                ++i_on_plane;
        }

        // p копланарен p:
        if (i_on_plane == number_of_vertexes) {
            if (p.normal() == splitter.normal())
                front_wl.add_polygon(p);
            else
                back_wl.add_polygon(p);
        }
        // p находится перед splitter:
        else if (i_front + i_on_plane == number_of_vertexes
                 && i_on_plane <= 2)
            front_wl.add_polygon(p);
        // p находится сзади splitter:
        else if (i_back + i_on_plane == number_of_vertexes
                 && i_on_plane <= 2)
            back_wl.add_polygon(p);
        // Иначе p пересекает плоскость splitter:
        else {
            world_layer tmp(p);
            geometry_helper::split_by_a_plane(tmp, 0, splitter);
            front_wl.add_polygon(tmp.m_polygons[0]);
            back_wl.add_polygon(tmp.m_polygons[1]);
        }
    }

    if (!front_wl.is_empty()) {
        bsp_node* const front = new bsp_node(a_node, front_wl);
        a_node->m_front = front;
        build_bsp_tree(front);
    }
    if (!back_wl.is_empty()) {
        bsp_node* const back = new bsp_node(a_node, back_wl);
        a_node->m_back = back;
        build_bsp_tree(back);
    }

    world_layer tmp(splitter);
    a_node->m_leaf_wl.clear();
    a_node->m_leaf_wl = std::move(tmp);
}


bsp_node*
bsp_compiler::compile(
        const world_layer& a_world_layer)
const noexcept
{
    // Флаг m_void будет использоваться для того чтобы узнать использовался ли
    // полигон в качестве сплиттера
    std::for_each(const_cast<world_layer&>(a_world_layer).m_polygons.begin(),
                  const_cast<world_layer&>(a_world_layer).m_polygons.end(),
                  [](polygon& a) {a.m_void = false;});

    bsp_node* root = new bsp_node(nullptr, a_world_layer);
    build_bsp_tree(root);
    std::cout << "done\n";
    return root;
}


bsp_node::bsp_node(bsp_node* a_parent, const world_layer& a_wl) noexcept
    : m_parent(a_parent), m_leaf_wl(a_wl)
{}

bsp_node::~bsp_node() noexcept
{
    if (m_back)
        delete m_back;
    if (m_front)
        delete m_front;
}

void bsp_node::render(const camera& a_cam) const noexcept
{
    if (!m_is_leaf) {
        float d = geometry_helper::distance(m_leaf_wl.m_polygons[0],
                                            vec3d(a_cam.pos()));
        if (d > 0) {
            if (m_back) m_back->render(a_cam);
            if (m_front) m_front->render(a_cam);
        }
        else {
            if (m_front) m_front->render(a_cam);
            if (m_back) m_back->render(a_cam);
        }
    }
    else
        m_leaf_wl.draw(a_cam);
}
