#pragma once


#include "Object.h"

class Blackhole : Object {
    public:
        float r_s; // schwartzschild radius
        float mass;
        Blackhole(vec2 pos, float m) : mass(m), Object(pos) {r_s = 2 * G * mass /(C * C);};
        void draw() override;
};