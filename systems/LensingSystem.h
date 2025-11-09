#ifndef SYSTEMS_LENSING_SYSTEM_H
#define SYSTEMS_LENSING_SYSTEM_H

#include "glm/glm.hpp"
#include "../core/System.h"
#include "../core/Coordinator.h"
#include "../components/GravityWell.h"
#include "../components/Projectile.h"
#include "../components/Transform2D.h"
#include "../components/Velocity2D.h"
#include "../components/Trail.h"

class Coordinator;
extern Coordinator coordinator;

// Physical constants
#define c 299792458.0f       // Speed of light in m/s
#define G 6.67430e-11f      // Gravitational constant
#define c2 (c*c)           // c²

struct GeodesicState {
    float r, phi;          // position
    float dr, dphi;        // velocities
    float E, L;           // conserved quantities
};

class LensingSystem : public System
{
public:
    void update(float);

private:
    void geodesicRHS(const GeodesicState& state, float rhs[4], float rs);
    void rk4Step(GeodesicState& state, float dl, float rs);
    void addState(const float a[4], const float b[4], float factor, float out[4]);
    void updatePosition(Transform2D& pos, Velocity2D& vel, GeodesicState& state);

    glm::vec4 rhs(glm::vec4 const &r_theta_dr_dtheta, float const &r_s);
};

#endif