
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

using namespace glm;
using namespace std;






class Engine {
    public:
        GLFWwindow *window;
        
        int WIDTH = 800;
        int HEIGHT = 600;
        float width = 1e11f; // Width of the viewport in meters
        float height = 7.5e10f;
        float dt = 1.0;
        static Engine& getInstance() {
            static Engine instance;
            return instance;
        }

        ~Engine() {
            glfwTerminate();
        }


    void run() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float left   = -width;
        float right  =  width;
        float bottom = -height;
        float top    =  height;
        glOrtho(left, right, bottom, top, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }


    private:
        Engine() {
            if (!glfwInit()) { cerr << "Failed to initialize GLFW\n"; exit(EXIT_FAILURE); }

            // Hints must be after glfwInit and before glfwCreateWindow
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // compatibility style

            window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", nullptr, nullptr);
            if (!window) { cerr << "Failed to create GLFW window\n"; glfwTerminate(); exit(EXIT_FAILURE); }

            glfwMakeContextCurrent(window);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                cerr << "Failed to initialize GLAD\n";
                exit(EXIT_FAILURE);
            }

            glViewport(0, 0, WIDTH, HEIGHT);
            glClearColor(0.f, 0.f, 0.f, 1.f); // background
        }


};
