#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "Engine.cpp"
#include "BlackHole.h"
#include "Ray.h"
using namespace glm;

void processInput(GLFWwindow *window);


vector<Ray> generateRays() {
    Engine& engine = Engine::getInstance();

    std::vector<Ray> retour;
    for(int i = 0 ; i<= 20 ; i++) {
        retour.push_back(Ray(vec2(-engine.width, (i - 10.0f) * engine.height / 10.0f), vec2(C,0)));
    }
    return retour;
}

int main()
{    
    Engine& engine = Engine::getInstance();

    Blackhole sagtA = Blackhole(vec2(0.0f, 0.0f), 8.54e36);
    std::vector<Ray> listOfRay = generateRays();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(engine.window))
    {

        engine.run();
        
        sagtA.draw();

        for(auto& r : listOfRay) {
            r.step(engine.dt);
            r.draw();
        }
        processInput(engine.window);

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
