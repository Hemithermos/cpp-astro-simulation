#pragma once


#include "Object.h"

class Blackhole : Object {
    public:
        double r_s; // schwartzschild radius
        double mass;
        Blackhole(vec2 pos, double m) : mass(m), Object(pos) {r_s = 2 * G * mass /(C * C);};
        void draw() override;
};