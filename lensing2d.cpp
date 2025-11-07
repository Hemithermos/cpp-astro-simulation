#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#define WIDTH 800
#define HEIGHT 600

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

    glViewport(0,0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    // vertex shader
    
    // unsigned int VBO; // vertex buffer object
    // glGenBuffers(1, &VBO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);  // link the VBO to the array buffer, so target buffer => target VBO
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // static => faster write on screen, slower set

    // we stored the vertices in the graphic card

    // vertex shader
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 1 is the number of strings, NULL is an array of strings length
    glCompileShader(vertexShader); // compile at run time

    // check vertex shader sanity
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // ========================

    // Fragment shader

    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    const char* fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\0";

    unsigned int fragmentShader, fragmentShader2; // same as before
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader); // compile at run time
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);
    // check fragment shader sanity
    int  success2;
    char infoLog2[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success2);

    if(!success2)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog2);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog2 << std::endl;
    }
    // ========================

    // shader program
    // its an object that link the shaders together
    unsigned int shaderProgam, shaderProgram2;
    shaderProgam = glCreateProgram(); // get the id of the program
    // link the shaders to the program
    glAttachShader(shaderProgam, vertexShader);
    glAttachShader(shaderProgam, fragmentShader);
    glLinkProgram(shaderProgam);

    shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(fragmentShader2);
    // check shader program sanity
    glGetProgramiv(shaderProgam, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgam, 512, NULL, infoLog);
    }
    // ===================


    // we tell OpenGL how to interpret the vertices we feed him
    // we have
    //        Vertex 1      Vertex 2      Vertex 3
    //       x   y   z    x    y    z    x    y   z
    //octet 0  4   8   12   16   20   24   28  32   36   => taille d'un vertex : 3 * sizeof(float) = 12
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*) 0);
    // 1->  location = 0 so start = 0
    // 2->  vec3 so 3 value
    // 3-> type of data : gl_float
    // 4-> bool, true if we want to normalize the given data
    // 5-> stride entre les attributs
    // 6-> type void* c'est l'offset du début dans l'array
    glEnableVertexAttribArray(0);

    float vertices1[] = {
        -0.5f,  -0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        0.0f, 0.0f, 0.0f,  // bottom left
    };

    float vertices2[] = {
        0.0f, 0.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,
        0.5f, 0.5f, 0.0f   // top left 
    };


    // thats kinda boring for only ONE (UNO 1) object
    // VAO = vertex array object, points to a vertex array and strides along the data to retrieve the good vertices

    // create and bind vao and vbo
    unsigned int VAO1, VBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1); // copy our vertices array in a buffer for OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0); // set the vertex attribute pointer
    glEnableVertexAttribArray(0);

    // call to vertexAttribPointer binded VBO as the vertex buffer object so we can clear the Buffer Array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // unbind the VAO 


    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2); // copy our vertices array in a buffer for OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0); // set the vertex attribute pointer
    glEnableVertexAttribArray(0);

    // call to vertexAttribPointer binded VBO as the vertex buffer object so we can clear the Buffer Array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // unbind the VAO 

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgam);

        // render 1st triangle
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // unbind the first VAO and bind the second, use the second shader

        glUseProgram(shaderProgram2);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgam);
    glDeleteProgram(shaderProgram2);

    glfwTerminate();
    return 0;
}