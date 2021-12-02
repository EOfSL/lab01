#ifndef BSP_H
#define BSP_H

#include <iterator>

#include "world_layer.h"


class bsp_tree
{
public:

    struct bsp_node
    {
        bsp_node* m_parent;
        bsp_node* m_front = nullptr;
        bsp_node* m_back = nullptr;

        explicit bsp_node(bsp_node*) noexcept;
        ~bsp_node() noexcept;

        void add_face(const polygon&) noexcept;
    };

    bsp_node m_root;

    bsp_node* root() noexcept;

    bsp_tree() noexcept;
    ~bsp_tree() noexcept;

    // Компиляция бсп-дерева для заданного слоя.
    using splitter_scorer = std::vector<polygon>::const_iterator (*)(const world_layer&);
    static bsp_tree* compile(const world_layer&, splitter_scorer);

    static std::vector<polygon>::const_iterator default_splitter_scorer(const world_layer&) noexcept;

protected:
    static void build_bsp_tree(bsp_node*, const world_layer&, splitter_scorer);
};

#endif // BSP_H
