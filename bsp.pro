TEMPLATE = app
CONFIG += console c++20a
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lglut -lGLU -lGL "/usr/lib/libGLEW.so"

SOURCES += \
        compilers/bsp_compiler.cpp \
        compilers/csg_compiler.cpp \
        geometry/brush.cpp \
        geometry/bsp_tree.cpp \
        geometry/geometry_helper.cpp \
        geometry/polygon.cpp \
        geometry/vec3d.cpp \
        geometry/world.cpp \
        geometry/world_layer.cpp \
        main.cpp \
        render/camera.cpp \
        render/material.cpp \
        render/shader.cpp \
        render/stb_image.cpp \
        render/texture.cpp \
        render/transform.cpp \
        window.cpp

HEADERS += \
    compilers/bsp_compiler.h \
    compilers/csg_compiler.h \
    geometry/brush.h \
    geometry/bsp_tree.h \
    geometry/geometry_helper.h \
    geometry/polygon.h \
    geometry/vec3d.h \
    geometry/world.h \
    geometry/world_layer.h \
    render/camera.h \
    render/material.h \
    render/shader.h \
    render/stb_image.h \
    render/texture.h \
    render/transform.h \
    window.h
