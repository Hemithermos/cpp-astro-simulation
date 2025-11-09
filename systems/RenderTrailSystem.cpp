#include "RenderTrailSystem.h"
#include "../core/Coordinator.h"
#include "../components/Trail.h"
#include "../components/Color.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <cmath>
#include <limits>
#include <iostream>

extern Coordinator coordinator;

void RenderTrailSystem::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Interleaved attributes: position (vec3) then color (vec4)
    const GLsizei stride = (3 + 4) * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderTrailSystem::setShader(std::shared_ptr<Shader> s)
{
    shader = s;
}

RenderTrailSystem::~RenderTrailSystem()
{
    // Only delete GL resources if an OpenGL context is still current.
    // glfwGetCurrentContext() returns NULL after glfwTerminate(), calling
    // GL functions without a context can cause a crash on program exit.
    if (glfwGetCurrentContext()) {
        if (VBO) glDeleteBuffers(1, &VBO);
        if (VAO) glDeleteVertexArrays(1, &VAO);
    }
}

void RenderTrailSystem::updateBuffers()
{
    vertexData.clear();

    for (Entity e : listOfEntities) {
        const Trail &trail = coordinator.getComponent<Trail>(e);
        if (trail.trail.size() < 2) continue;

        glm::vec4 baseCol = glm::vec4(1.0f);
        if (coordinator.hasComponent<Color>(e)) {
            baseCol = coordinator.getComponent<Color>(e).color;
        }

        size_t N = trail.trail.size();
        // Build segments as pairs of vertices so we can draw with GL_LINES in one draw call
        for (size_t i = 0; i < N - 1; ++i) {
            // alpha fades from older (0.1) to newer (1.0)
            float t = float(i) / float(std::max<size_t>(1, N - 1));
            float alpha = 0.1f + 0.9f * t;

            glm::vec4 col = baseCol;
            col.a *= alpha;

            // first point
            vertexData.push_back(trail.trail[i].x);
            vertexData.push_back(trail.trail[i].y);
            vertexData.push_back(0.0f);
            vertexData.push_back(col.r);
            vertexData.push_back(col.g);
            vertexData.push_back(col.b);
            vertexData.push_back(col.a);

            // second point
            vertexData.push_back(trail.trail[i + 1].x);
            vertexData.push_back(trail.trail[i + 1].y);
            vertexData.push_back(0.0f);
            vertexData.push_back(col.r);
            vertexData.push_back(col.g);
            vertexData.push_back(col.b);
            vertexData.push_back(col.a);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!vertexData.empty()) {
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_DYNAMIC_DRAW);
    } else {
        // keep buffer small if no data
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderTrailSystem::renderTrails()
{
    if (!shader) return;

    updateBuffers();

    shader->use();

    // Aspect correction same as RenderSpheresSystem
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect = 1.0f;
    if (viewport[3] != 0) aspect = (float)viewport[2] / (float)viewport[3];
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect, 1.0f, 1.0f));
    shader->setTransform("transform", transform);

    if (vertexData.empty()) return;





    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    // Ensure the VBO is bound so attribute pointers read from the correct buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexData.size() / 7));
    // Check for GL errors to help debug bad draws
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "RenderTrailSystem: GL error after draw: 0x" << std::hex << err << std::dec << "\n";
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
