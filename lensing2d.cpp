
#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "core/Shader.h"
#include <cmath>
#define WIDTH 800
#define HEIGHT 600

struct VAOinfo {
    unsigned int VAO;
    unsigned int VBO;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

VAOinfo createVAO(float* array, size_t arraySize,
                        GLuint locV, GLint sizeV, GLenum typeV, GLboolean normalizeV, GLsizei strideV, void* offsetV,
                        GLuint locF, GLint sizeF, GLenum typeF, GLboolean normalizeF, GLsizei strideF, void* offsetF)
{
    VAOinfo info;
    glGenVertexArrays(1, &info.VAO);
    glGenBuffers(1, &info.VBO);
    glBindVertexArray(info.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, info.VBO); // copy our vertices array in a buffer for OpenGL
    glBufferData(GL_ARRAY_BUFFER, arraySize, array, GL_STATIC_DRAW);

    glVertexAttribPointer(locV, sizeV, typeV, normalizeV, strideV, offsetV);
    glEnableVertexAttribArray(locV);
    glVertexAttribPointer(locF, sizeF, typeF, normalizeF, strideF, offsetF);
    glEnableVertexAttribArray(locF);


    // call to vertexAttribPointer binded VBO as the vertex buffer object so we can clear the Buffer Array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // unbind the VAO 

    return info;
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

    glViewport(0,0, WIDTH, HEIGHT);
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

    float vertices[] = {
        -0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top , red
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left, green
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // bottom right, blue
    };



    // thats kinda boring for only ONE (UNO 1) object
    // VAO = vertex array object, points to a vertex array and strides along the data to retrieve the good vertices

    // create and bind vao and vbo


    VAOinfo vaovbo = createVAO(vertices, sizeof(vertices),
                            0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) 0,
                            1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)( 3*sizeof(float)));

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f,0.2f,0.2f, 1.0f);
        processInput(window);
        ourShader.use();

        // render 1st triangle
        glBindVertexArray(vaovbo.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // unbind the first VAO and bind the second, use the second shader


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &vaovbo.VAO);
    glDeleteBuffers(1, &vaovbo.VBO);


    glfwTerminate();
    return 0;
}


