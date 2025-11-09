#ifndef CORE_SHADER_H
#define CORE_SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

struct VAOinfo {
    GLuint VAO;
    GLuint VBO;
};


class Shader
{
public:
    // shader id
    unsigned int ID;

    // constructor to read the shaders
    Shader(const char*, const char*);
    // activate the shader using glUseProgram
    void use();
    // utils uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setTransform(const std::string &name, glm::mat4 value) const;
    void setVec4(const std::string &name, glm::vec4 value) const;



};

#endif