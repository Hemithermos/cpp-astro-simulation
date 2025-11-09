#ifndef SYSTEMS_RENDERTRAILSYSTEM_H
#define SYSTEMS_RENDERTRAILSYSTEM_H

#include <memory>
#include "../core/System.h"
#include "../core/Shader.h"
#include <glad/glad.h>
#include <vector>

class RenderTrailSystem : public System {
public:
    void init();
    void renderTrails();
    void setShader(std::shared_ptr<Shader> shader);
    ~RenderTrailSystem();

private:
    GLuint VAO, VBO;
    std::shared_ptr<Shader> shader;
    std::vector<float> vertexData;
    void updateBuffers();
};

#endif