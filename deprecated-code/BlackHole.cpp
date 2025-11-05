#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "BlackHole.h"

void Blackhole::draw() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        const int segments = 100;
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * M_PI * i / segments;
            float x = r_s * cosf(theta);
            float y = r_s * sinf(theta); 
            glVertex2f(x, y);
        }
    glEnd();
}
