#ifndef WORLD_LAYER_H
#define WORLD_LAYER_H

#include <list>
#include <vector>
#include <initializer_list>

#include "polygon.h"


/**
 * @brief Мировой слой -- это хранилище некое полигонов. Содержит вершины и
 * полигоны между ними.
 *
 * Где используется:
 * 1. По одному экземпляру лежит в каждом браше. Это -- полигоны, относящиеся
 * к данному брашу. Объединение всех этих слоёв даёт полную геометрию мира.
 * 2. Как временное хранилище полигонов, например для выполнения сечения.
 * 3. По одному экземпляру лежит в каждом листе бсп-дерева после его компиляции.
 */
class world_layer
{
public:
    std::list<vertex> m_vertexes;
    std::vector<polygon> m_polygons;

    world_layer() noexcept;
    explicit world_layer(const polygon&) noexcept;
    explicit world_layer(const std::vector<vertex>&) noexcept;
    explicit world_layer(const std::vector<vertex*>&) noexcept;
    world_layer(const world_layer&) noexcept;
    world_layer(world_layer&&) noexcept;
    world_layer& operator=(const world_layer&) noexcept;
    world_layer& operator=(world_layer&&) noexcept;
    ~world_layer() noexcept;

    const std::list<vertex>& vertexes() const noexcept;
    const std::vector<polygon>& polygons() const noexcept;

    // Добавление элементов
    vertex* add_vertex(const vertex&) noexcept;
    void add_polygon(const polygon&) noexcept;
    void add_polygon(const std::initializer_list<vertex>&) noexcept;
    void add_layer(const world_layer&) noexcept;

    // Отчистить
    void clear() noexcept;

    bool is_empty() const noexcept;

    // Отрисовка
    void prepare_to_draw();
    void draw(const camera&) const noexcept;

    // Объединение полигонов, исключения если это невозможно. Новый полигон
    // имеет свойства первого. Вернёт true если удалось и false иначе.
    bool merge(polygon&, const polygon&);

    // Статические фабрики
    static world_layer box(const vec3d&,
                           float,
                           float,
                           float,
                           bool,
                           const material*,
                           const material*,
                           const material*);
    static world_layer quad(const vec3d& a_offset,
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
                            const material* a_material_cellar);
};

#endif // WORLD_LAYER_H
