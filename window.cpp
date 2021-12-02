#include "window.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <stdexcept>

#include "render/camera.h"
#include "geometry/world.h"
#include "compilers/csg_compiler.h"
#include "compilers/bsp_compiler.h"


window* g_main_window;

void window::pressKey(unsigned char key, int x, int y) noexcept
{
    bool& m_camera_moving_mode = g_main_window->m_camera_moving_mode;

    if (m_camera_moving_mode) {
        switch (key) {
        case 'd': g_main_window->m_delta_x -= window::m_speed; break;
        case 'a': g_main_window->m_delta_x += window::m_speed; break;
        case 'w': g_main_window->m_delta_z += window::m_speed; break;
        case 's': g_main_window->m_delta_z -= window::m_speed; break;
        case ' ': g_main_window->m_delta_y += window::m_speed; break;
        }
    }
    if (key == 27) {
        m_camera_moving_mode = !m_camera_moving_mode;
        g_main_window->m_prev_ax = x;
        g_main_window->m_prev_ay = y;
        g_main_window->m_delta_x = 0;
        g_main_window->m_delta_y = 0;
        g_main_window->m_delta_z = 0;
//        glutSetCursor(m_camera_moving_mode ? GLUT_CURSOR_NONE : GLUT_CURSOR_LEFT_ARROW);
    }
}

void window::releaseKey(unsigned char key, int, int) noexcept
{
    if (g_main_window->m_camera_moving_mode) {
        switch (key) {
        case 'd': g_main_window->m_delta_x += window::m_speed; break;
        case 'a': g_main_window->m_delta_x -= window::m_speed; break;
        case 'w': g_main_window->m_delta_z -= window::m_speed; break;
        case 's': g_main_window->m_delta_z += window::m_speed; break;
        case ' ': g_main_window->m_delta_y -= window::m_speed; break;
        }
    }
}

void window::pressSKey(int key, int, int) noexcept
{
    switch (key) {
    case 114:
        if (g_main_window->m_camera_moving_mode)
            g_main_window->m_delta_y -= window::m_speed;
        break;
    }
}

void window::releaseSKey(int key, int, int) noexcept
{
    switch (key) {
    case 114:
        if (g_main_window->m_camera_moving_mode)
            g_main_window->m_delta_y += window::m_speed;
        break;
    }
}

void window::mouseClick(int, int, int, int) noexcept
{

}

void window::mouseMove(int x, int y) noexcept
{
    if (!g_main_window->m_camera_moving_mode)
        return;

    // Просто указатели
    float& m_yaw = g_main_window->m_yaw;
    float& m_pitch = g_main_window->m_pitch;
    // Рассчёты
    const float offset_ax = x - g_main_window->m_prev_ax;
    const float offset_ay = g_main_window->m_prev_ay - y;
    g_main_window->m_prev_ax = x;
    g_main_window->m_prev_ay = y;
    m_yaw   += offset_ax * g_main_window->m_sensitivity;
    m_pitch += offset_ay * g_main_window->m_sensitivity;

    // Ограничение на поворот по вертикали
    if (m_pitch > 89)
        m_pitch = 89;
    else if (m_pitch < -89)
        m_pitch = -89;

    // Избежание переполнения горизонтального угла
    if (m_yaw > 360)
        m_yaw += -360;
    else if (m_yaw < -360)
        m_yaw += 360;

    // Поворот камеры
    glm::vec3 dir;
    dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    dir.y = sin(glm::radians(m_pitch));
    dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    g_main_window->m_camera.m_forward = dir;

//    if (std::fabs(x - g_main_window->m_width / 2) > g_main_window->m_width / 2 ||
//            std::fabs(y - g_main_window->m_height / 2) > g_main_window->m_height / 2) {
//        glutWarpPointer(g_main_window->m_width / 2, g_main_window->m_height / 2);
//    }
}

void window::display() noexcept
{
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef ZBUFFER
    g_main_window->m_bsp.draw(g_main_window->m_camera);
#else
    g_main_window->m_bsp->render(g_main_window->m_camera);
#endif

    glFlush();

    // Движение камеры
    g_main_window->m_camera.m_position +=
            g_main_window->m_delta_x * g_main_window->m_camera.right()  +
            g_main_window->m_delta_y * g_main_window->m_camera.m_up     +
            g_main_window->m_delta_z * g_main_window->m_camera.m_forward;
}

void window::resize(int a_width, int a_height) noexcept
{
    if (a_height == 0)
        a_height = 1;

    // Использовать матрицу проекции
    glMatrixMode(GL_PROJECTION);

    // Обнулить матрицу проекции
    glLoadIdentity();

    // Параметры вьюпорта
    glViewport(0, 0, a_width, a_height);

    // Новая перспектива
    g_main_window->m_camera.m_projection_matrix =
            glm::perspective(90.0f,
                             static_cast<float>(a_width) / a_height,
                             0.1f,
                             1000.0f);

    g_main_window->m_width = a_width;
    g_main_window->m_height = a_height;

    // Вернуться к матрице проекции
    glMatrixMode(GL_MODELVIEW);
}

window::window(int argc, char** argv) noexcept
{
    g_main_window = this;

    // Окно
    m_width = 1200;
    m_height = 900;

    glutInit(&argc, argv);
    // Окно
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(g_main_window->m_width, g_main_window->m_height);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Main");
    // Инициализация glew
    auto er = glewInit();
    if (GLEW_OK != er)
        std::cout << "glew упал: " << glewGetErrorString(er) << std::endl;
    std::cout << "glew: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "glut: " << GLUT_API_VERSION << std::endl;


    // Функция тика
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(resize);

    // Хандлер клавиш
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(pressKey);
    glutKeyboardUpFunc(releaseKey);
    glutSpecialFunc(pressSKey);
    glutSpecialUpFunc(releaseSKey);
    // Хандлер мыши
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMove);

    // Прочие настройки opengl
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Камера
    m_camera.m_position = glm::vec3(0, 0, 0);
    m_camera.m_forward = glm::vec3(0, 0, 1);
    m_camera.m_up = glm::vec3(0, 1, 0);
    m_camera.m_projection_matrix =
            glm::perspective(90.0f,
                             static_cast<float>(g_main_window->m_width) / g_main_window->m_height,
                             0.1f,
                             1000.0f);

    // =========================================================
    // =========================================================
    // Материалы
    const material bricks("./textures/bricks.png");
    const material cobblestone("./textures/cobblestone.png");
    const material end_stone_bricks("./textures/end_stone_bricks.png");
    const material oak_planks("./textures/oak_planks.png");
    const material polished_andesite("./textures/polished_andesite.png");
    const material polished_basalt_top("./textures/polished_basalt_top.png");
    const material polished_diorite("./textures/polished_diorite.png");
    const material polished_granite("./textures/polished_granite.png");
    const material purpur_pillar("./textures/purpur_pillar.png");
    const material quartz_pillar("./textures/quartz_pillar.png");
    const material stone_bricks("./textures/stone_bricks.png");
    const material stone("./textures/stone.png");
    // Браши
    world w;

//#define SIMPLE_SCENE

#ifdef SIMPLE_SCENE
    w.add_brush(brush::box(vec3d(0, 0, 0), 5, 3, 12, false, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    w.add_brush(brush::box(vec3d(-2, 0, 3), 2, 2, 2, false, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
#else
    vec3d offset;
#define door_sn(offset) \
    w.add_brush(brush::box(offset, 0.5, 2, 2, false, \
        &stone_bricks, &polished_basalt_top, &polished_basalt_top)); \
    w.add_brush(brush::quad(offset + vec3d(0, 2, 0), \
                            vec3d(0, 0, 0), \
                            vec3d(0, 0, 2), \
                            vec3d(0, 0.7, 0.5), \
                            vec3d(0, 0.7, 1.5), \
                            vec3d(0.5, 0, 0), \
                            vec3d(0.5, 0, 2), \
                            vec3d(0.5, 0.7, 0.5), \
                            vec3d(0.5, 0.7, 1.5), \
                            false, &polished_basalt_top))

    // Комната A
    offset = vec3d(0, 0, 0);
    w.add_brush(brush::box(offset + vec3d(0, 0, 0), 5, 3, 12, false, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    // Белые колонны
    w.add_brush(brush::box(offset + vec3d(0, 0, 0), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(0, 0, 3), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(0, 0, 6), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(0, 0, 10), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(4.5, 0, 0), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(4.5, 0, 3), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(4.5, 0, 6), 0.5, 3, 1, true, &quartz_pillar));
    w.add_brush(brush::box(offset + vec3d(4.5, 0, 10), 0.5, 3, 1, true, &quartz_pillar));
    // Потолок
    w.add_brush(brush::quad(offset + vec3d(1, 3, 0),
                            vec3d(0, 0, 0),
                            vec3d(0, 0, 12),
                            vec3d(1, 1, 0),
                            vec3d(1, 1, 12),
                            vec3d(3, 0, 0),
                            vec3d(3, 0, 12),
                            vec3d(2, 1, 0),
                            vec3d(2, 1, 12),
                            false, &oak_planks));

    // Комната B
    offset = vec3d(5.5, -2, 0);
    w.add_brush(brush::box(offset + vec3d(0, 0, 0), 12, 6, 12, false, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    w.add_brush(brush::box(offset + vec3d(0, 0, 6), 12, 2, 6, true, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    w.add_brush(brush::box(offset + vec3d(0, 0, 3), 3, 2, 3, true, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    // Ступеньки
    w.add_brush(brush::box(offset + vec3d(3, 0, 3), 0.5, 1.5, 3, true, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    w.add_brush(brush::box(offset + vec3d(3.5, 0, 3), 0.5, 1.0, 3, true, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    w.add_brush(brush::box(offset + vec3d(4, 0, 3), 0.5, 0.5, 3, true, &stone_bricks, &polished_basalt_top, &polished_basalt_top));
    // Окна и дверь
    offset = vec3d(0, 0, 0);
    w.add_brush(brush::box(offset + vec3d(5, 0.5, 1.5), 0.5, 2, 1, false, &stone_bricks));
    w.add_brush(brush::box(offset + vec3d(5, 0.5, 4.5), 0.5, 2, 1, false, &stone_bricks));
    door_sn(offset + vec3d(5, 0, 7.5));
#endif

    // Компиляция csg
    csg_compiler csg;
    csg.compile(w);

#ifdef ZBUFFER
    m_bsp = w.get_full_world_layer();
    m_bsp.prepare_to_draw();
#else
    // Компиляция bsp
    bsp_compiler bsp;
    m_bsp = bsp.compile(w.get_full_world_layer());
#endif

    glutMainLoop();
}

window::~window() noexcept
{
#ifdef ZBUFFER
#else
    delete m_bsp;
#endif
}
