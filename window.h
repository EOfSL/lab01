#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include "render/camera.h"
#include "compilers/bsp_compiler.h"


class window;

extern window* g_main_window;

class window
{
public:
    camera m_camera;

//#define ZBUFFER
#ifdef ZBUFFER
    world_layer m_bsp;
#else
    bsp_node* m_bsp;
#endif

    unsigned m_width, m_height;

    bool m_camera_moving_mode = false;
    static constexpr float m_speed       = 0.0025f;
    static constexpr float m_sensitivity = 0.35f;
    float m_delta_x = 0, m_delta_y = 0, m_delta_z = 0;
    float m_prev_ax = 0, m_prev_ay = 0;
    float m_yaw = 90, m_pitch = 0; // Именно 90 градусов

    window(int argc, char** argv) noexcept;
    ~window() noexcept;

    static void display() noexcept;
    static void resize(int, int) noexcept;
    static void pressKey(unsigned char, int, int) noexcept;
    static void releaseKey(unsigned char, int, int) noexcept;
    static void pressSKey(int, int, int) noexcept;
    static void releaseSKey(int, int, int) noexcept;
    static void mouseClick(int, int, int, int) noexcept;
    static void mouseMove(int, int) noexcept;
};


#endif // WINDOW_H
