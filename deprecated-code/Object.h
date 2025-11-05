#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

static const double C = 299792458.0;
static const double G = 6.67430e-11;

class Object {
    private:


    public:
    vec2 position;
    virtual void draw() = 0;
    Object(vec2 pos) : position(pos) {};
};

