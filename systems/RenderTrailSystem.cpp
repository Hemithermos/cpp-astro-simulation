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

void RenderTrailSystem::generateTrailPoints(std::vector<GLfloat>& pointList, std::vector<glm::vec3> trail, glm::vec3 pos, glm::vec4 color)
{

    pointList.push_back(pos.x);
    pointList.push_back(pos.y);
    pointList.push_back(pos.z);
    pointList.push_back(color.x);
    pointList.push_back(color.y);
    pointList.push_back(color.z);
    pointList.push_back(1.0f);

    size_t lengthOfTrail = trail.size();
    if(lengthOfTrail == 0) return;
    for(size_t i = lengthOfTrail - 1 ; i > 0; i--)
    {
        pointList.push_back(trail[i].x);
        pointList.push_back(trail[i].y);
        pointList.push_back(trail[i].z);

        pointList.push_back(color.x);
        pointList.push_back(color.y);
        pointList.push_back(color.z);
        pointList.push_back((float) i / lengthOfTrail);
    }

}

VAOinfo RenderTrailSystem::setUpTrailBuffers(const std::vector<GLfloat> &vertices)
{
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

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

void RenderTrailSystem::setShader(std::shared_ptr<Shader> s)
{
    shader = s;
}




void RenderTrailSystem::renderTrails()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(Entity e : listOfEntities)
    {
        auto pos = coordinator.getComponent<Transform2D>(e).position;
        auto trail = coordinator.getComponent<Trail>(e).trail;
        glm::vec4 col = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        if (coordinator.hasComponent<Color>(e)) {
            col = coordinator.getComponent<Color>(e).color;
        }

        std::vector<GLfloat> coords;
        generateTrailPoints(coords, trail, glm::vec3(pos, 0.0f), col);
        VAOinfo vaovbo = setUpTrailBuffers(coords);

        shader->use();

        glBindVertexArray(vaovbo.VAO);
        glDrawArrays(GL_LINE_STRIP, 0, coords.size()/7);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &vaovbo.VAO);
        glDeleteBuffers(1, &vaovbo.VBO);
    }
}
