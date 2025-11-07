#pragma once

#include "Object.h"
#include <vector>

class Ray : Object {
    public:
        vec2 speed;
        std::vector<vec2> trail;
        Ray(vec2 pos, vec2 spd) : speed(spd), Object(pos) {}; 
        void draw() override;
        void step(float dt);


};