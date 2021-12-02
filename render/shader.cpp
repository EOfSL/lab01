#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>


// Записать файл в строку
std::string load_file(const std::string&) noexcept;
// Создать шейдер из файла
GLuint create_shader(const std::string&, GLenum) noexcept;
// Вывести ошибку шейдеров
void check_shader_error(GLuint, GLuint, bool, const std::string&) noexcept;


shader::shader(const std::string& a_file)
{
    // Создание программы шейдера
    m_program = glCreateProgram();

    // Создание двух шейдеров, вертексного и фрагментного, из файлов
    m_shaders[0] = create_shader(load_file(a_file + ".vs"), GL_VERTEX_SHADER);
    m_shaders[1] = create_shader(load_file(a_file + ".fs"), GL_FRAGMENT_SHADER);

    // Заявить программе об обоих шейдерах
    for (size_t i = 0; i < NUM_SHADERS; ++i)
        glAttachShader(m_program, m_shaders[i]);

    glBindAttribLocation(m_program, 0, "position");
    glBindAttribLocation(m_program, 1, "uv");
    glBindAttribLocation(m_program, 2, "normal");

    glLinkProgram(m_program);
    check_shader_error(m_program, GL_LINK_STATUS, true, "Программа не смогла связаться: ");

    glValidateProgram(m_program);
    check_shader_error(m_program, GL_VALIDATE_STATUS, true, "Программа невалидна: ");

    m_uniforms[TRANSFORM_U] = glGetUniformLocation(m_program, "transform");
    m_uniforms[DIFFUSE_U] = glGetUniformLocation(m_program, "diffuse");
}

void shader::bind() const noexcept
{
    glUseProgram(m_program);
}

void shader::update(const camera& a_cam) const noexcept
{
    const glm::mat4 MVPMatrix = a_cam.view_projection_matrix();
    glUniformMatrix4fv(m_uniforms[TRANSFORM_U], 1, GL_FALSE, &MVPMatrix[0][0]);
}

shader::~shader()
{
    // Отвязать и удалить шейдеры
    for (size_t i = 0; i < NUM_SHADERS; i++) {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }
    // Удалить программу
    glDeleteProgram(m_program);
}

std::string load_file(const std::string& a_file) noexcept
{
    std::ifstream file(a_file);

    if (!file.is_open())
        std::cerr << "Ошибка открытия файла: " << a_file << std::endl;

    std::stringstream b;
    b << file.rdbuf();

    return b.str();
}

GLuint create_shader(const std::string& a_text, GLenum a_shader_type) noexcept
{
    const GLuint shader = glCreateShader(a_shader_type);

    if (shader == 0)
        std::cerr << "Ошибка компиляции шейдера нод номером " << a_shader_type << std::endl;

    const GLchar* shaderSourceStrings[1];
    shaderSourceStrings[0] = a_text.c_str();
    GLint shaderSourceStringsLengths[1];
    shaderSourceStringsLengths[0] = a_text.length();

    glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLengths);
    glCompileShader(shader);

    check_shader_error(shader, GL_COMPILE_STATUS, false, "Ошибка компиляции шейдера");

    return shader;
}

void check_shader_error(GLuint a_shader, GLuint a_flag, bool a_is_program, const std::string& a_error_message) noexcept
{
    GLint success = 0;
    GLchar error[1024] = {0};

    if (a_is_program)
        glGetProgramiv(a_shader, a_flag, &success);
    else
        glGetShaderiv(a_shader, a_flag, &success);

    if (success == GL_FALSE) {
        if (a_is_program)
            glGetProgramInfoLog(a_shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(a_shader, sizeof(error), NULL, error);

        std::cerr << a_error_message << ": '" << error << "'" << std::endl;
    }
}
