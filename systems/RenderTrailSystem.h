#ifndef SYSTEMS_RENDERTRAILSYSTEM_H
#define SYSTEMS_RENDERTRAILSYSTEM_H

#include <memory>
#include "../core/System.h"
#include "../core/Shader.h"
#include <glad/glad.h>
#include <vector>
#include "../components/Trail.h"
#include "../components/Transform2D.h"

class RenderTrailSystem : public System {
public:
    void renderTrails();

    // generate interleaved vertex data: [pos.x,pos.y,pos.z, r,g,b,a, ...]
    void generateTrailPoints(std::vector<GLfloat>&, std::vector<glm::vec3>, glm::vec3, glm::vec4);

    VAOinfo setUpTrailBuffers(const std::vector<GLfloat>&);
    void setShader(std::shared_ptr<Shader> shader);

private:
    std::shared_ptr<Shader> shader;
};

#endif