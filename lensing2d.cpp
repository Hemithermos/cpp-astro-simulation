
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


#include "systems/RenderSpheresSystem.h"
#include "core/Shader.h"
#include "core/Coordinator.h"

#define WIDTH 800
#define HEIGHT 600

// Global coordinator instance referenced by systems via `extern Coordinator coordinator;`
Coordinator coordinator;





void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
    
    const char* vpath = "shaders/vertexShader.glsl";
    const char* fpath = "shaders/fragmentShader.glsl";
    Shader ourShader(vpath, fpath);
    // dont uncomment if no uniform
    // ourShader.setFloat("SomeUniform", 1.0f);


    // we tell OpenGL how to interpret the vertices we feed him
    // we have
    //        Vertex 1      Vertex 2      Vertex 3
    //       x   y   z    x    y    z    x    y   z
    //octet 0  4   8   12   16   20   24   28  32   36   => taille d'un vertex : 3 * sizeof(float) = 12
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


    auto sphereSys = coordinator.registerSystem<RenderSpheresSystem>();
    {
        Signature signature;
        signature.set(coordinator.getComponentType<Transform2D>());
        signature.set(coordinator.getComponentType<Spherical>());
        coordinator.setSystemSignature<RenderSpheresSystem>(signature);
    }
    sphereSys->setShader(std::make_shared<Shader>(ourShader));

    Entity entity = coordinator.createEntity();
    coordinator.addComponent<Transform2D>(entity, {glm::vec2(0.0f, 0.0f)});
    coordinator.addComponent<Spherical>(entity, {1.0f});
    coordinator.addComponent<Color>(entity, {glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)});




    // thats kinda boring for only ONE (UNO 1) object
    // VAO = vertex array object, points to a vertex array and strides along the data to retrieve the good vertices

    // create and bind vao and vbo


    glClearColor(0.2f,0.2f,0.2f, 1.0f);


    while(!glfwWindowShouldClose(window))
    {
        
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        sphereSys->renderCircle();



        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glfwTerminate();
    return 0;
}


