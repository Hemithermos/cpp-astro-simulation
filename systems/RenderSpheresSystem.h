#ifndef SYSTEM_RENDER_SPHERES_SYSTEM_H
#define SYSTEM_RENDER_SPHERES_SYSTEM_H

#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "../core/System.h"
#include "../components/Spherical.h"
#include "../components/Transform2D.h"
#include "../core/Shader.h"
#include "../core/Coordinator.h"

class Coordinator;
class Shader;
extern Coordinator coordinator;

class RenderSpheresSystem : public System
{
public:
    
    void renderCircle(int numPoints = 100);
    VAOinfo setupCircleBuffers(const std::vector<GLfloat> &);
    // generate interleaved vertex data: [pos.x,pos.y,pos.z, r,g,b,a, ...]
    std::vector<GLfloat> generateCirclePoints(float radius, glm::vec3 pos, glm::vec4 color, int numpoints = 100);
    void setShader(std::shared_ptr<Shader> s) {shader = s;};

private:
    std::shared_ptr<Shader> shader;
};

#endif