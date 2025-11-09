
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

    const char *vpath = "shaders/vertexShader.glsl";
    const char *fpath = "shaders/fragmentShader.glsl";
    auto sharedShader = std::make_shared<Shader>(vpath, fpath);
    // dont uncomment if no uniform
    // ourShader.setFloat("SomeUniform", 1.0f);

    // we tell OpenGL how to interpret the vertices we feed him
    // we have
    //        Vertex 1      Vertex 2      Vertex 3
    //       x   y   z    x    y    z    x    y   z
    // octet 0  4   8   12   16   20   24   28  32   36   => taille d'un vertex : 3 * sizeof(float) = 12
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*) 0);

    // 1->  location = 0 so start = 0
    // 2->  vec3 so 3 value
    // 3-> type of data : gl_float
    // 4-> bool, true if we want to normalize the given data
    // 5-> stride entre les attributs
    // 6-> type void* c'est l'offset du début dans l'array
    // glEnableVertexAttribArray(0);

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
    trailSys->init();
    trailSys->setShader(sharedShader);

    // register lensing physical system and set its signature (entities with Transform2D)
    auto lensSys = coordinator.registerSystem<LensingSystem>();
    {
        Signature lsign;
        lsign.set(coordinator.getComponentType<Transform2D>());
        coordinator.setSystemSignature<LensingSystem>(lsign);
    }

    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    float aspect = (fbh > 0) ? float(fbw) / float(fbh) : 1.0f;

    const float halfWorldX = ww;                  // meters to show in +X and -X
    const float halfWorldY = hw; // match scale so circles stay round

    Entity blackHole = coordinator.createEntity();
    coordinator.addComponent<Transform2D>(blackHole, {glm::vec2(0.0f, 0.0f)});
    float rs = 2.0f * G * 8.54e36f / (c * c);             // Schwarzschild radius
    coordinator.addComponent<Spherical>(blackHole, {rs}); // Visual size matches Schwarzschild radius
    coordinator.addComponent<Color>(blackHole, {glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)});
    coordinator.addComponent<GravityWell>(blackHole, {8.54e36f, rs});

    // for(int i = 0 ; i <= 11 ; i++)

    // thats kinda boring for only ONE (UNO 1) object
    // VAO = vertex array object, points to a vertex array and strides along the data to retrieve the good vertices

    // create and bind vao and vbo

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    const int rayCount = 11;         // choose what you like
    const float xLeft = -2*halfWorldX; // left edge in world meters
    const float yBottom = -2*halfWorldY;
    const float yTop = 2*halfWorldY;
    const float yStep = (rayCount > 1) ? (yTop - yBottom) / float(rayCount - 1) : 0.0f;

    for (int i = 0; i < rayCount; ++i)
    {
        float y = yBottom + i * yStep; // evenly spaced across full height
        Entity r = coordinator.createEntity();
        coordinator.addComponent<Transform2D>(r, {glm::vec2(xLeft, y)});
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
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        float aspect = (fbh > 0) ? float(fbw) / float(fbh) : 1.0f;

        const float halfWorldX = ww;
        const float halfWorldY = halfWorldX / aspect;

        const float sx = 1.0f / (2.0f * halfWorldX);
        const float sy = 1.0f / (2.0f * halfWorldY);
        glm::mat4 world = glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, 1.0f));
        sharedShader->use();
        sharedShader->setTransform("worldTransform", world);

        processInput(window);
        lensSys->update(1.0f);
        sphereSys->renderCircle(/*numPoints*/); // no per-system aspect scaling
        trailSys->renderTrails();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
