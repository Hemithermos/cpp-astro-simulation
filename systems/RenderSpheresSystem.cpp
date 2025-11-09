#include "RenderSpheresSystem.h"
#include "../components/Color.h"
#include <glm/gtc/matrix_transform.hpp>



void RenderSpheresSystem::renderCircle(int numPoints)
{
    for(Entity e : listOfEntities)
    {
        auto pos = coordinator.getComponent<Transform2D>(e).position;
        auto radius = coordinator.getComponent<Spherical>(e).radius;
        glm::vec4 col = glm::vec4(1.0f);
        if (coordinator.hasComponent<Color>(e)) {
            col = coordinator.getComponent<Color>(e).color;
        }

        std::vector<GLfloat> coords = generateCirclePoints(radius, glm::vec3(pos, 0.0f), col, numPoints);
        VAOinfo vaovbo = setupCircleBuffers(coords);

        shader->use();
        // compute aspect correction transform so circles look round
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        float aspect = 1.0f;
        if (viewport[3] != 0) aspect = (float)viewport[2] / (float)viewport[3];
        // scale X by 1/aspect to compensate for non-square viewport
        glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect, 1.0f, 1.0f));
        shader->setTransform("transform", transform);

        glBindVertexArray(vaovbo.VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, numPoints + 2);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &vaovbo.VAO);
        glDeleteBuffers(1, &vaovbo.VBO);
    }
}   

VAOinfo RenderSpheresSystem::setupCircleBuffers(const std::vector<GLfloat> &vertices)
{
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Interleaved attributes: position (vec3) then color (vec4)
    const GLsizei stride = (3 + 4) * sizeof(GLfloat);
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAOinfo{VAO, VBO};
}

std::vector<GLfloat> RenderSpheresSystem::generateCirclePoints(float radius, glm::vec3 pos, glm::vec4 color, int numPoints) {
    // interleaved array: [x,y,z, r,g,b,a, x,y,z, r,g,b,a, ...]
    std::vector<GLfloat> vertices;

    // center vertex
    vertices.push_back(pos.x);
    vertices.push_back(pos.y);
    vertices.push_back(pos.z);
    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);
    vertices.push_back(color.a);

    // ring vertices; include duplicate of first vertex at the end to close the fan
    for (int i = 0; i <= numPoints; i++) {
        float angle = 2.0f * M_PI * i / numPoints;
        vertices.push_back(pos.x + radius * cos(angle));  // x
        vertices.push_back(pos.y + radius * sin(angle));  // y
        vertices.push_back(pos.z);                        // z
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(color.a);
    }

    return vertices;
}

