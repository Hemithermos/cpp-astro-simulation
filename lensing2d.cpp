
#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "components/Spherical.h"
#include "components/Transform2D.h"
#include "components/Color.h"
#include "components/GravityWell.h"
#include "components/Projectile.h"
#include "components/Trail.h"
#include "components/Velocity2D.h"

#include "systems/RenderSpheresSystem.h"
#include "systems/LensingSystem.h"
#include "systems/RenderTrailSystem.h"
#include "core/Shader.h"
#include "core/Coordinator.h"

#define WIDTH 800
#define HEIGHT 600
#define ww 100000000000.0f // 100 billion meters (1e11)
#define hw 75000000000.0f  // 75 billion meters (7.5e10)
#define c 299792458.0f     // Speed of light in m/s
// Global coordinator instance referenced by systems via `extern Coordinator coordinator;`
Coordinator coordinator;


glm::mat4 projection;

bool isPaused = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    float windowAspect = (float)width / (float)height;
    float worldAspect = ww / hw;

    if (windowAspect > worldAspect) {
        // Window is wider than world -> expand world width
        float newW = hw * windowAspect;
        projection = glm::ortho(-newW, newW, -hw, hw, -1.0f, 1.0f);
    } else {
        // Window is taller than world -> expand world height
        float newH = ww / windowAspect;
        projection = glm::ortho(-ww, ww, -newH, newH, -1.0f, 1.0f);
    }

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        isPaused = !isPaused;
        std::cout << (isPaused ? "Paused\n" : "Resumed\n");
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Lensing in 2 dimensions", NULL, NULL);
    if (window == NULL)
    {

        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the OpenGL context current before loading GL function pointers
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    const char *vpath = "shaders/vertexShader.glsl";
    const char *fpath = "shaders/fragmentShader.glsl";
    auto sharedShader = std::make_shared<Shader>(vpath, fpath);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // 4) Create proper orthographic projection that matches our world coordinates
    float left =   - ww;    // Left edge of world
    float right =  + ww;   // Right edge of world
    float bottom = - hw; // Bottom edge of world
    float top =    + hw;     // Top edge of world

    // Orthographic projection matrix
    projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);



    // Coordinator of ECS
    coordinator.init();

    coordinator.registerComponent<Spherical>();
    coordinator.registerComponent<Color>();
    coordinator.registerComponent<Transform2D>();
    coordinator.registerComponent<GravityWell>();
    coordinator.registerComponent<Projectile>();
    coordinator.registerComponent<Velocity2D>();
    coordinator.registerComponent<Trail>();

    // Only use RenderSpheresSystem for the black hole
    auto sphereSys = coordinator.registerSystem<RenderSpheresSystem>();
    {
        Signature signature;
        signature.set(coordinator.getComponentType<Transform2D>());
        signature.set(coordinator.getComponentType<Spherical>());
        signature.set(coordinator.getComponentType<GravityWell>()); // Only for black hole
        coordinator.setSystemSignature<RenderSpheresSystem>(signature);
    }
    sphereSys->setShader(sharedShader);

    // Create trail system for photons
    auto trailSys = coordinator.registerSystem<RenderTrailSystem>();
    {
        Signature signature;
        signature.set(coordinator.getComponentType<Transform2D>());
        signature.set(coordinator.getComponentType<Trail>());
        signature.set(coordinator.getComponentType<Color>());
        coordinator.setSystemSignature<RenderTrailSystem>(signature);
    }
    trailSys->setShader(sharedShader);

    // register lensing physical system and set its signature (entities with Transform2D)
    auto lensSys = coordinator.registerSystem<LensingSystem>();
    {
        Signature lsign;
        lsign.set(coordinator.getComponentType<Transform2D>());
        coordinator.setSystemSignature<LensingSystem>(lsign);
    }



    Entity blackHole = coordinator.createEntity();
    coordinator.addComponent<Transform2D>(blackHole, {glm::vec2(0.0f, 0.0f)});
    float rs = 2.0f * G * 8.54e36f / (c * c);             // Schwarzschild radius
    coordinator.addComponent<Spherical>(blackHole, {rs}); // Visual size matches Schwarzschild radius
    coordinator.addComponent<Color>(blackHole, {glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)});
    coordinator.addComponent<GravityWell>(blackHole, {8.54e36f, rs});


    int rayCount = 100;
    const float yStep = (rayCount > 1) ? (top - bottom) / float(rayCount - 1) : 0.0f;

    for (int i = 0; i < rayCount; ++i)
    {
        float y = bottom + i * yStep; // evenly spaced across full height
        Entity r = coordinator.createEntity();
        coordinator.addComponent<Transform2D>(r, {glm::vec2(left, y)});
        coordinator.addComponent<Velocity2D>(r, {glm::vec2(c, 0.0f)}); // to the right at c
        Projectile p;
        p.impactParameter = std::fabs(y);
        coordinator.addComponent<Projectile>(r, p);
        coordinator.addComponent<Color>(r, {glm::vec4(1, 1, 0, 1)});
        coordinator.addComponent<Trail>(r, {});
    }

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        sharedShader->use();
        sharedShader->setTransform("projection", projection);

        if(!isPaused)
        {
            lensSys->update(1.5f);
        }

        // Render black hole
        sphereSys->renderCircle(100); // 100 points pour un plus joli cercle

        // Render trails
        trailSys->renderTrails();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
